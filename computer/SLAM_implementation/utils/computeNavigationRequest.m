function cmdLine = computeNavigationRequest(x, y, heading, goalX, goalY, params)
% Pose from SLAM (m, m, rad). Goal in same frame. params: stopDist,
% maxMoveStep, maxRotateStep, rotateThreshold (rad). Returns "" at goal.

    cmdLine = "";

    dx = goalX - x;
    dy = goalY - y;
    dist = hypot(dx, dy);

    if dist < params.stopDist
        return;
    end

    bearingToGoal = atan2(dy, dx);
    headingError = wrapToPi(bearingToGoal - heading);

    if abs(headingError) > params.rotateThreshold
        rot = max(-params.maxRotateStep, min(params.maxRotateStep, headingError));
        cmdLine = sprintf("rotate,%.4f", rot);
        return;
    end

    step = min(params.maxMoveStep, dist);
    cmdLine = sprintf("move,%.4f", step);
end
