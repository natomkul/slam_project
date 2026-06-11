function drawMap(occMap, ekfHist, poseGraphHist, x_est)
    cla;
    show(occMap);
    hold on;
    
    plot(ekfHist(1,:), ekfHist(2,:), 'r-', 'LineWidth', 2, ...
        'DisplayName', 'Trajektoria estymowana przez EKF');
    
    if ~isempty(poseGraphHist)
        plot(poseGraphHist(1,:), poseGraphHist(2,:), 'g-', 'LineWidth', 2, ...
            'DisplayName', 'Trajektoria robota');
    end
    
    plot(x_est(1), x_est(2), 'ro', 'MarkerFaceColor', '#F2A696', ...
        'DisplayName', 'Aktualna pozycja robota', 'MarkerSize', 10);

    quiver(x_est(1), x_est(2), ...
       0.25*cos(x_est(3)), 0.25*sin(x_est(3)), ...
       0, 'b', 'LineWidth', 2, 'MaxHeadSize', 2, ...
       'DisplayName', 'Kierunek jazdy');
    
    legend("Location", "northeast");
    grid on;
    axis equal;
    xlabel('x [m]');
    ylabel('y [m]');
    title('Trajektoria robota oraz mapa');
    drawnow;
end