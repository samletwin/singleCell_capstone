#ifndef TOUCH_H
#define TOUCH_H

/* Defines */
#define FT53XX_DEFAULT_ADDR 0x38 //!< I2C address
#define FT53XX_REG_VENDID 0xA3   //!< FocalTech's panel ID
#define FT53XX_REG_FIRMVERS 0xA6 //!< Firmware version
#define FT53XX_REG_CHIPID 0xA8   //!< Chip selecting

#define FT53XX_VENDID 0x11 //!< FocalTech's panel ID
#define FT5336_CHIPID 0x79 //!< Chip selecting

#define FT53XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT5336_REG_THRESHGROUP 0x80 //!< Threshold for touch detection

#define FT5336_TD_STATUS 0x02 ///<! Touch status
#define FT5336_GEST_ID 0x01   ///<! Detected Gesture ID
#define FT5336_TOUCH1_XH 0x03 ///<! High byte of X touch #1
#define FT5336_TOUCH1_XL 0x04 ///<! Low byte of X touch #1
#define FT5336_TOUCH1_YH 0x05 ///<! High byte of Y touch #1
#define FT5336_TOUCH1_YL 0x06 ///<! Low byte of Y touch #1

#define FT53XX_NUM_X 0x33 //!< Touch X position
#define FT53XX_NUM_Y 0x34 //!< Touch Y position

#define FT53XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT53XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT53XX_REG_WORKMODE 0x00    //!< Work mode
#define FT53XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT53XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT53XX_REG_POINTRATE 0x88   //!< Point rate
/* API Functions */
void touch_setup();
void touch_loop();

#endif 