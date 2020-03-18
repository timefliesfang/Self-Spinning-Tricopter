#include "AC_AttitudeControl_Multi.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>
#include <GCS_MAVLink/GCS.h>

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl_Multi::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_AttitudeControl, 0),

    // @Param: RAT_RLL_P
    // @DisplayName: Roll axis rate controller P gain
    // @Description: Roll axis rate controller P gain.  Converts the difference between desired roll rate and actual roll rate into a motor speed output
    // @Range: 0.05 0.5
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_RLL_I
    // @DisplayName: Roll axis rate controller I gain
    // @Description: Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate
    // @Range: 0.01 2.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_IMAX
    // @DisplayName: Roll axis rate controller I gain maximum
    // @Description: Roll axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_RLL_D
    // @DisplayName: Roll axis rate controller D gain
    // @Description: Roll axis rate controller D gain.  Compensates for short-term change in desired roll rate vs actual roll rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FF
    // @DisplayName: Roll axis rate controller feed forward
    // @Description: Roll axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FILT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_roll, "RAT_RLL_", 1, AC_AttitudeControl_Multi, AC_PID),

    // @Param: RAT_PIT_P
    // @DisplayName: Pitch axis rate controller P gain
    // @Description: Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output
    // @Range: 0.05 0.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_PIT_I
    // @DisplayName: Pitch axis rate controller I gain
    // @Description: Pitch axis rate controller I gain.  Corrects long-term difference in desired pitch rate vs actual pitch rate
    // @Range: 0.01 2.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_IMAX
    // @DisplayName: Pitch axis rate controller I gain maximum
    // @Description: Pitch axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_PIT_D
    // @DisplayName: Pitch axis rate controller D gain
    // @Description: Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FF
    // @DisplayName: Pitch axis rate controller feed forward
    // @Description: Pitch axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FILT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_pitch, "RAT_PIT_", 2, AC_AttitudeControl_Multi, AC_PID),

    // @Param: RAT_YAW_P
    // @DisplayName: Yaw axis rate controller P gain
    // @Description: Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output
    // @Range: 0.10 2.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_YAW_I
    // @DisplayName: Yaw axis rate controller I gain
    // @Description: Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate
    // @Range: 0.010 1.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_IMAX
    // @DisplayName: Yaw axis rate controller I gain maximum
    // @Description: Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_YAW_D
    // @DisplayName: Yaw axis rate controller D gain
    // @Description: Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate
    // @Range: 0.000 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FF
    // @DisplayName: Yaw axis rate controller feed forward
    // @Description: Yaw axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FILT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 10
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_yaw, "RAT_YAW_", 3, AC_AttitudeControl_Multi, AC_PID),

    // @Param: THR_MIX_MIN
    // @DisplayName: Throttle Mix Minimum
    // @Description: Throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.25
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MIN", 4, AC_AttitudeControl_Multi, _thr_mix_min, AC_ATTITUDE_CONTROL_MIN_DEFAULT),

    // @Param: THR_MIX_MAX
    // @DisplayName: Throttle Mix Maximum
    // @Description: Throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAX", 5, AC_AttitudeControl_Multi, _thr_mix_max, AC_ATTITUDE_CONTROL_MAX_DEFAULT),

    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAN", 6, AC_AttitudeControl_Multi, _thr_mix_man, AC_ATTITUDE_CONTROL_MAN_DEFAULT),

    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.9
    // @User: Advanced
    AP_GROUPINFO("A", 7, AC_AttitudeControl_Multi, a, 0.6f),
    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.9
    // @User: Advanced
    AP_GROUPINFO("B", 8, AC_AttitudeControl_Multi, b, 0.6f),

    AP_GROUPINFO("D", 9, AC_AttitudeControl_Multi, d, 0.0f),

    AP_GROUPINFO("C", 10, AC_AttitudeControl_Multi, c, 0.0f),

    AP_GROUPINFO("A_D", 11, AC_AttitudeControl_Multi, dynamic_a, 0.3f),

    AP_GROUPINFO("B_D", 12, AC_AttitudeControl_Multi, dynamic_b, 0.3f),

    AP_GROUPEND
};

AC_AttitudeControl_Multi::AC_AttitudeControl_Multi(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt) :
    AC_AttitudeControl(ahrs, aparm, motors, dt),
    _motors_multi(motors),
    _pid_rate_roll(AC_ATC_MULTI_RATE_RP_P, AC_ATC_MULTI_RATE_RP_I, AC_ATC_MULTI_RATE_RP_D, AC_ATC_MULTI_RATE_RP_IMAX, AC_ATC_MULTI_RATE_RP_FILT_HZ, dt),
    _pid_rate_pitch(AC_ATC_MULTI_RATE_RP_P, AC_ATC_MULTI_RATE_RP_I, AC_ATC_MULTI_RATE_RP_D, AC_ATC_MULTI_RATE_RP_IMAX, AC_ATC_MULTI_RATE_RP_FILT_HZ, dt),
    _pid_rate_yaw(AC_ATC_MULTI_RATE_YAW_P, AC_ATC_MULTI_RATE_YAW_I, AC_ATC_MULTI_RATE_YAW_D, AC_ATC_MULTI_RATE_YAW_IMAX, AC_ATC_MULTI_RATE_YAW_FILT_HZ, dt)
{
    AP_Param::setup_object_defaults(this, var_info);
}

// Update Alt_Hold angle maximum
void AC_AttitudeControl_Multi::update_althold_lean_angle_max(float throttle_in)
{
    // calc maximum tilt angle based on throttle
    float thr_max = _motors_multi.get_throttle_thrust_max();

    // divide by zero check
    if (is_zero(thr_max)) {
        _althold_lean_angle_max = 0.0f;
        return;
    }

    float althold_lean_angle_max = acosf(constrain_float(_throttle_in/(AC_ATTITUDE_CONTROL_ANGLE_LIMIT_THROTTLE_MAX * thr_max), 0.0f, 1.0f));
    _althold_lean_angle_max = _althold_lean_angle_max + (_dt/(_dt+_angle_limit_tc))*(althold_lean_angle_max-_althold_lean_angle_max);
}

void AC_AttitudeControl_Multi::set_throttle_out(float throttle_in, bool apply_angle_boost, float filter_cutoff)
{
    _throttle_in = throttle_in;
    update_althold_lean_angle_max(throttle_in);
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    if (apply_angle_boost) {
        // Apply angle boost
        throttle_in = get_throttle_boosted(throttle_in);
    }else{
        // Clear angle_boost for logging purposes
        _angle_boost = 0.0f;
    }
    _motors.set_throttle(throttle_in);
    _motors.set_throttle_avg_max(get_throttle_avg_max(MAX(throttle_in, _throttle_in)));
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Multi::get_throttle_boosted(float throttle_in)
{
    if (!_angle_boost_enabled) {
        _angle_boost = 0;
        return throttle_in;
    }
    // inverted_factor is 1 for tilt angles below 60 degrees
    // inverted_factor reduces from 1 to 0 for tilt angles between 60 and 90 degrees

    float cos_tilt = _ahrs.cos_pitch() * _ahrs.cos_roll();
    float inverted_factor = constrain_float(2.0f*cos_tilt, 0.0f, 1.0f);
    float boost_factor = 1.0f/constrain_float(cos_tilt, 0.5f, 1.0f);

    float throttle_out = throttle_in*inverted_factor*boost_factor;
    _angle_boost = constrain_float(throttle_out - throttle_in,-1.0f,1.0f);
    return throttle_out;
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Multi::get_throttle_avg_max(float throttle_in)
{
    throttle_in = constrain_float(throttle_in, 0.0f, 1.0f);
    return MAX(throttle_in, throttle_in*MAX(0.0f,1.0f-_throttle_rpy_mix)+_motors.get_throttle_hover()*_throttle_rpy_mix);
}

// update_throttle_rpy_mix - slew set_throttle_rpy_mix to requested value
void AC_AttitudeControl_Multi::update_throttle_rpy_mix()
{
    // slew _throttle_rpy_mix to _throttle_rpy_mix_desired
    if (_throttle_rpy_mix < _throttle_rpy_mix_desired) {
        // increase quickly (i.e. from 0.1 to 0.9 in 0.4 seconds)
        _throttle_rpy_mix += MIN(2.0f*_dt, _throttle_rpy_mix_desired-_throttle_rpy_mix);
    } else if (_throttle_rpy_mix > _throttle_rpy_mix_desired) {
        // reduce more slowly (from 0.9 to 0.1 in 1.6 seconds)
        _throttle_rpy_mix -= MIN(0.5f*_dt, _throttle_rpy_mix-_throttle_rpy_mix_desired);
    }
    _throttle_rpy_mix = constrain_float(_throttle_rpy_mix, 0.1f, AC_ATTITUDE_CONTROL_MAX);
}

void AC_AttitudeControl_Multi::rate_controller_run()
{
    // move throttle vs attitude mixing towards desired (called from here because this is conveniently called on every iteration)
    update_throttle_rpy_mix();
    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    /** TODO: we modify the rate_controller because the angular velocity in the original code is based on the euler angle coordinate
    and thus does not apply to our coordinate. Therefore, transformation is necessary    
    **/
    Vector3f motor_in,motor_in_copy,mix;

    if(_rotation_enable){
        rate_bf_to_motor_roll_pitch(motor_in,gyro_latest,_rate_target_ang_vel);
    }
    if(_rotation_enable_copy){
        rate_bf_to_motor_roll_pitch_copy(motor_in_copy,gyro_latest,_rate_target_ang_vel);
    }
    else if(_rotation_enable==0&&_rotation_enable_copy==0){
        motor_in.x=rate_target_to_motor_roll(0.0f, _rate_target_ang_vel.x);
        motor_in.y=rate_target_to_motor_pitch(0.0f, _rate_target_ang_vel.y);
        motor_in.z=rate_target_to_motor_yaw(0.0f, _rate_target_ang_vel.z);

        motor_in_copy.x=rate_target_to_motor_roll_copy(gyro_latest.x, 0.0f);
        motor_in_copy.y=rate_target_to_motor_pitch_copy(gyro_latest.y, 0.0f);
        motor_in_copy.z=rate_target_to_motor_yaw_copy(gyro_latest.z, 0.0f);    
    }
    

    /*mix.x = a*motor_in.x + b*motor_in_copy.x;
    mix.y = a*motor_in.y + b*motor_in_copy.y;
    mix.z = a*motor_in.z + b*motor_in_copy.z;*/
    if(_attitude_target_euler_angle.x/M_PI*180>1||_attitude_target_euler_angle.z/M_PI*180>1){
        mix.x = motor_in.x*cosf(heading + period  +c*gyro_latest.z +d)*(a+dynamic_a) + motor_in.y*sinf(heading + period + c*gyro_latest.z +d)*a + motor_in_copy.x*(b-dynamic_b);
        mix.y =-motor_in.x*sinf(heading + period + c*gyro_latest.z +d)*(a+dynamic_a) + motor_in.y*cosf(heading + period + c*gyro_latest.z +d)*a + motor_in_copy.y*(b-dynamic_b);    
    }
    else{
        mix.x = motor_in.x*cosf(heading + period  +c*gyro_latest.z +d)*a + motor_in.y*sinf(heading + period + c*gyro_latest.z +d)*a + motor_in_copy.x*b;
        mix.y =-motor_in.x*sinf(heading + period + c*gyro_latest.z +d)*a + motor_in.y*cosf(heading + period + c*gyro_latest.z +d)*a + motor_in_copy.y*b;
    }
    
    float roll1 = motor_in.x*cosf(heading + period + d) + motor_in.y*sinf(heading + period + d);
    float pitch1 =-motor_in.x*sinf(heading + period + d) + motor_in.y*cosf(heading + period + d);
    mix.z = 0.0f;
    static int counter;
    counter++;
    if(counter%50==0){
        if(_tag==0)
            gcs().send_text(MAV_SEVERITY_CRITICAL, "R:%.3f  P:%.3f",mix.x,mix.y);
        else if(_tag==1)
            gcs().send_text(MAV_SEVERITY_CRITICAL, "R1:%.3f  P1:%.3f",roll1,pitch1);
        else if(_tag==2)
            gcs().send_text(MAV_SEVERITY_CRITICAL, "R2:%.3f  P2:%.3f",motor_in_copy.x,motor_in_copy.y);
        else if(_tag==3){
            gcs().send_text(MAV_SEVERITY_CRITICAL, "%d:*%.3f *%.3f *%.3f *%.3f",counter,roll1,pitch1);
            gcs().send_text(MAV_SEVERITY_CRITICAL, "*%.3f  *%.3f",motor_in_copy.x,motor_in_copy.y);
        }   
        else if(_tag==4)
            gcs().send_text(MAV_SEVERITY_CRITICAL, "l:%.0f x:%.0f y:%.0f p:%.0f h:%.0f",lean_angle*180/M_PI,lean_angle_x*180/M_PI,lean_angle_y*180/M_PI,period*180/M_PI,heading*180/M_PI);
        else if(_tag==5){
            gcs().send_text(MAV_SEVERITY_CRITICAL, "%d:*%.0f  *%.0f  *%.0f  *%.0f",counter,lean_angle_x*180/M_PI,lean_angle_y*180/M_PI,period*180/M_PI,heading*180/M_PI);
            gcs().send_text(MAV_SEVERITY_CRITICAL, "*%.3f  *%.3f  *%.3f  *%.3f",roll1,pitch1,motor_in_copy.x,motor_in_copy.y);
            gcs().send_text(MAV_SEVERITY_CRITICAL, "*%.3f  *%.3f",mix.x,mix.y);
        }
    }
    constrain_float(mix.x, -1.0f, 1.0f);
    constrain_float(mix.y, -1.0f, 1.0f);
    constrain_float(mix.z, -1.0f, 1.0f);
    _motors.set_roll(mix.x);
    _motors.set_pitch(mix.y);
    _motors.set_yaw(mix.z);
    /**t fcm 0318 +end**/
    /**t fcm 0318 -**/
      /*_motors.set_roll(rate_target_to_motor_roll(gyro_latest.x, _rate_target_ang_vel.x));
    _motors.set_pitch(rate_target_to_motor_pitch(gyro_latest.y, _rate_target_ang_vel.y));
    _motors.set_yaw(rate_target_to_motor_yaw(gyro_latest.z, _rate_target_ang_vel.z));*/
    /**t fcm 0318 -end**/

    control_monitor_update();
}

// sanity check parameters.  should be called once before takeoff
void AC_AttitudeControl_Multi::parameter_sanity_check()
{
    // sanity check throttle mix parameters
    if (_thr_mix_man < 0.1f || _thr_mix_man > 4.0f) {
        // parameter description recommends thr-mix-man be no higher than 0.9 but we allow up to 4.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_man.set_and_save(AC_ATTITUDE_CONTROL_MAN_DEFAULT);
    }
    if (_thr_mix_min < 0.1f || _thr_mix_min > 0.25f) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
    }
    if (_thr_mix_max < 0.5f || _thr_mix_max > AC_ATTITUDE_CONTROL_MAX) {
        // parameter description recommends thr-mix-max be no higher than 0.9 but we allow up to 5.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
    if (_thr_mix_min > _thr_mix_max) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
}

