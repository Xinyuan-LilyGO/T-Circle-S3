/* T-Circle-S3 IMU 示例：自动选择 ICM20948 或 QMI8658 + QMC6309。 */
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "ICM20948_WE.h"
#include "SensorQMI8658.hpp"
#include "SensorQMC6309.hpp"
#include "pin_config.h"

ICM20948_WE icm20948(ICM20948_ADDRESS);
SensorQMI8658 qmi8658;
SensorQMC6309 qmc6309;
enum class ImuKind { kIcm20948, kQmi8658Qmc6309 };
ImuKind imu_kind;

bool InitQmi8658Qmc6309() {
    if (!qmi8658.begin(Wire, QMI8658_H_SLAVE_ADDRESS, QMI8658_SDA, QMI8658_SCL) ||
        !qmc6309.begin(Wire, QMC6309_SLAVE_ADDRESS, QMC6309_SDA, QMC6309_SCL)) {
        return false;
    }
    if (!qmi8658.configAccelerometer(SensorQMI8658::ACC_RANGE_2G,
                                     SensorQMI8658::ACC_ODR_125Hz,
                                     SensorQMI8658::LPF_MODE_3) ||
        !qmi8658.configGyroscope(SensorQMI8658::GYR_RANGE_256DPS,
                                  SensorQMI8658::GYR_ODR_112_1Hz,
                                  SensorQMI8658::LPF_MODE_3) ||
        !qmi8658.disableSyncSampleMode()) {
        return false;
    }
    qmi8658.enableAccelerometer();
    qmi8658.enableGyroscope();
    return qmc6309.setOperationMode(OperationMode::CONTINUOUS_MEASUREMENT) &&
           qmc6309.setFullScaleRange(MagFullScaleRange::FS_32G) &&
           qmc6309.setOversamplingRate(MagOverSampleRatio::OSR_8) &&
           qmc6309.setLowPassFilter(MagLowPassFilter::LPF_8) &&
           qmc6309.setSetResetMode(SensorQMC6309::MagSetResetMode::SET_AND_RESET_ON);
}

bool InitImu() {
    Wire.setPins(ICM20948_SDA, ICM20948_SCL);
    Wire.begin();
    if (icm20948.init() && icm20948.initMagnetometer()) {
        icm20948.sleep(false);
        icm20948.setAccRange(ICM20948_ACC_RANGE_2G);
        icm20948.setAccDLPF(ICM20948_DLPF_6);
        icm20948.setMagOpMode(AK09916_CONT_MODE_20HZ);
        imu_kind = ImuKind::kIcm20948;
        Serial.println("IMU selected: ICM20948");
        return true;
    }
    if (InitQmi8658Qmc6309()) {
        imu_kind = ImuKind::kQmi8658Qmc6309;
        Serial.println("IMU selected: QMI8658 + QMC6309");
        return true;
    }
    return false;
}

void PrintIcm20948() {
    icm20948.readSensor();
    const xyzFloat acceleration = icm20948.getGValues();
    const xyzFloat angles = icm20948.getAngles();
    const xyzFloat magnetic = icm20948.getMagValues();
    const float yaw = atan2f(magnetic.y, magnetic.x) * 180.0f / PI;
    Serial.printf("ACC (g): %.3f %.3f %.3f\n", acceleration.x, acceleration.y, acceleration.z);
    Serial.printf("ANGLE (deg): %.2f %.2f %.2f, YAW %.2f\n", angles.x, angles.y, angles.z, yaw);
    Serial.printf("MAG (uT): %.3f %.3f %.3f\n", magnetic.x, magnetic.y, magnetic.z);
}

void PrintQmi8658Qmc6309() {
    if (qmi8658.getDataReady()) {
        IMUdata acc = {};
        IMUdata gyr = {};
        if (qmi8658.getAccelerometer(acc.x, acc.y, acc.z) && qmi8658.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            Serial.printf("ACC (m/s2): %.3f %.3f %.3f\n", acc.x, acc.y, acc.z);
            Serial.printf("GYR (dps): %.3f %.3f %.3f\n", gyr.x, gyr.y, gyr.z);
        }
    }
    MagnetometerData magnetic;
    if (qmc6309.readData(magnetic)) {
        Serial.printf("MAG (uT): %.3f %.3f %.3f%s\n", magnetic.magnetic_field.x * 100.0f,
                      magnetic.magnetic_field.y * 100.0f, magnetic.magnetic_field.z * 100.0f,
                      magnetic.overflow ? " (overflow)" : "");
    }
}

void setup() {
    Serial.begin(115200);
    delay(20);
    if (!InitImu()) {
        Serial.println("No supported IMU found");
        while (true) delay(1000);
    }
}

void loop() {
    if (imu_kind == ImuKind::kIcm20948) PrintIcm20948();
    else PrintQmi8658Qmc6309();
    delay(100);
}
