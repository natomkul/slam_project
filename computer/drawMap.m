function drawMap(occMap, ekf_hist, pg_hist, x_est)

    cla;
    show(occMap);
    hold on;
    
    plot(ekf_hist(1,:), ekf_hist(2,:), 'r-', ...
        'LineWidth', 2, ...
        'DisplayName', 'EKF robot position estimate');
    
    if ~isempty(pg_hist)
        plot(pg_hist(1,:), pg_hist(2,:), 'g-', ...
            'LineWidth', 2, ...
            'DisplayName', 'Pose-Graph SLAM');
    end
    
    plot(x_est(1), x_est(2), 'ro', ...
        'MarkerFaceColor', 'r', ...
        'DisplayName', 'Current EKF pose');
    
    legend("Location", "best");
    grid on;
    axis equal;
    xlabel('x [m]');
    ylabel('y [m]');
    title('EKF + LiDAR Pose-Graph SLAM + Occupancy Map');
    drawnow;
    
end