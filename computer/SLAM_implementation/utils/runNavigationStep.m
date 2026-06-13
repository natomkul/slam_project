function runNavigationStep(cmdClient, navFig, x_est, occMap, navParams)
% runNavigationStep  Path follow + move/rotate commands when user set a goal (map click).

    if ~isNavigationGoalActive(navFig)
        return;
    end

    goalX = getappdata(navFig, 'navGoalX');
    goalY = getappdata(navFig, 'navGoalY');

    replanNavigationIfNeeded(navFig, x_est, occMap, goalX, goalY);

    waypoints = getappdata(navFig, 'navWaypoints');
    wpIdx = getappdata(navFig, 'navWaypointIdx');
    if wpIdx > size(waypoints, 1)
        return;
    end

    targetX = waypoints(wpIdx, 1);
    targetY = waypoints(wpIdx, 2);

    distToTarget = hypot(targetX - x_est(1), targetY - x_est(2));
    if distToTarget < navParams.stopDist
        wpIdx = wpIdx + 1;
        setappdata(navFig, 'navWaypointIdx', wpIdx);
        if wpIdx > size(waypoints, 1)
            return;
        end
        targetX = waypoints(wpIdx, 1);
        targetY = waypoints(wpIdx, 2);
    end

    publishNavigationStep(cmdClient, x_est, targetX, targetY, navParams);
end
