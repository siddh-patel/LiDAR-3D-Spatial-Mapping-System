
#define I2C_MCS_ACK 0x00000008    // Data Acknowledge Enable
#define I2C_MCS_DATACK 0x00000008 // Acknowledge Data
#define I2C_MCS_ADRACK 0x00000004 // Acknowledge Address
#define I2C_MCS_STOP 0x00000004   // Generate STOP
#define I2C_MCS_START 0x00000002  // Generate START
#define I2C_MCS_ERROR 0x00000002  // Error
#define I2C_MCS_RUN 0x00000001    // I2C Master Enable
#define I2C_MCS_BUSY 0x00000001   // I2C Busy
#define I2C_MCR_MFE 0x00000010    // I2C Master Function Enable

#define MAXRETRIES 5 // number of receive attempts before giving up

void I2C_Init(void);

// The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void);

// XSHUT        This pin is an active-low shutdown input;
//              the board pulls it up to VDD to enable the sensor by default.
//              Driving this pin low puts the sensor into hardware standby. This input is not level-shifted.
void VL53L1X_XSHUT(void);
