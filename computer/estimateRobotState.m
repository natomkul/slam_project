function [r_next, F] = estimateRobotState(r, u, en_dt, imu_dt, L)
    %r_next - estimated robot state
    %F - state transition

    %r - currect robot state
    %u - input [distanceR, distanceL, gyro, accelerationX, accelerationY]
    %en_dt - time dif for encoder data
    %imu_dt - time dif for imu data

    distanceR = u(1);
    distanceL = u(2);
    gyro = u(3);

    distance = (distanceR + distanceL)/2;

    angle = r(3);
    final_angle = angle + gyro * imu_dt;

    if en_dt > 0
        v_new = distance / en_dt;
    else
        v_new = r(4);
    end

    r_next = [
        r(1) + distance * cos(final_angle);
        r(2) + distance * sin(final_angle);
        wrapToPi(final_angle);
        v_new
    ];

    F = [
        1 0 -distance * sin(final_angle)    0;
        0 1  distance * cos(final_angle)    0;
        0 0  1                              0;
        0 0  0                              0
    ];
end