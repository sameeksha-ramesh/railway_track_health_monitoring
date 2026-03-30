/**
 * @file    gps_parser.h
 * @brief   Lightweight NMEA GPS parser for NEO-6M on TI CC3200
 * @author  Sameeksha R
 *
 * Parses $GPRMC and $GPGGA NMEA sentences from Serial1 stream.
 * Extracts: latitude, longitude, speed (knots → km/h).
 */
 
#ifndef GPS_PARSER_H
#define GPS_PARSER_H
 
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
 
#define GPS_BUFFER_SIZE  128
#define KNOTS_TO_KMPH    1.852f
 
class GPSParser {
public:
    GPSParser() : _lat(0), _lon(0), _speed(0), _valid(false) {
        memset(_buf, 0, sizeof(_buf));
        _idx = 0;
    }
 
    /**
     * @brief  Feed one byte from Serial into the parser.
     *         Call inside loop() while Serial1.available().
     */
    void encode(char c) {
        if (c == '$') { _idx = 0; }         /* Start of new sentence */
        if (_idx < GPS_BUFFER_SIZE - 1) {
            _buf[_idx++] = c;
            _buf[_idx]   = '\0';
        }
        if (c == '\n') { _parse(); }         /* End of sentence */
    }
 
    float latitude()   const { return _lat;   }
    float longitude()  const { return _lon;    }
    float speed_kmph() const { return _speed;  }
    bool  is_valid()   const { return _valid;  }
 
private:
    char  _buf[GPS_BUFFER_SIZE];
    int   _idx;
    float _lat, _lon, _speed;
    bool  _valid;
 
    /** Extract the Nth comma-separated field from _buf into out[] */
    bool _field(int n, char *out, int maxlen) {
        int field = 0, pos = 0;
        for (int i = 0; _buf[i]; i++) {
            if (_buf[i] == ',') { field++; pos = 0; continue; }
            if (field == n && pos < maxlen - 1) out[pos++] = _buf[i];
        }
        out[pos] = '\0';
        return pos > 0;
    }
 
    /** Convert NMEA ddmm.mmmm → decimal degrees */
    float _nmea_to_deg(float nmea) {
        int   deg  = (int)(nmea / 100);
        float mins = nmea - deg * 100.0f;
        return deg + mins / 60.0f;
    }
 
    void _parse() {
        char tmp[32];
 
        /* ── $GPRMC — recommended minimum sentence ── */
        if (strncmp(_buf, "$GPRMC", 6) == 0) {
            /* Field 2: status (A=active, V=void) */
            _field(2, tmp, sizeof(tmp));
            _valid = (tmp[0] == 'A');
            if (!_valid) return;
 
            /* Field 3: latitude */
            _field(3, tmp, sizeof(tmp));
            float raw_lat = atof(tmp);
            _lat = _nmea_to_deg(raw_lat);
 
            /* Field 4: N/S */
            _field(4, tmp, sizeof(tmp));
            if (tmp[0] == 'S') _lat = -_lat;
 
            /* Field 5: longitude */
            _field(5, tmp, sizeof(tmp));
            float raw_lon = atof(tmp);
            _lon = _nmea_to_deg(raw_lon);
 
            /* Field 6: E/W */
            _field(6, tmp, sizeof(tmp));
            if (tmp[0] == 'W') _lon = -_lon;
 
            /* Field 7: speed in knots → km/h */
            _field(7, tmp, sizeof(tmp));
            _speed = atof(tmp) * KNOTS_TO_KMPH;
        }
    }
};
 
#endif /* GPS_PARSER_H */
