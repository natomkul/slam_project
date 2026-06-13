function replanNavigationIfNeeded(navFig, x_est, occMap, goalX, goalY)
% replanNavigationIfNeeded  Rebuild path when goal moves or previous route finished.

    replan = false;

    if consumeNavigationGoalChanged(navFig)
        replan = true;
    elseif ~isappdata(navFig, 'navWaypoints')
        replan = true;
    elseif ~isappdata(navFig, 'navPlannedGoalX')
        replan = true;
    else
        plannedX = getappdata(navFig, 'navPlannedGoalX');
        plannedY = getappdata(navFig, 'navPlannedGoalY');
        if hypot(goalX - plannedX, goalY - plannedY) > 0.02
            replan = true;
        end
    end

    if ~replan
        return;
    end

    waypoints = planPath(occMap, [x_est(1), x_est(2)], [goalX, goalY]);
    if isempty(waypoints)
        warning("No path to goal (%.2f, %.2f)", goalX, goalY);
        waypoints = [x_est(1), x_est(2); goalX, goalY];
    end

    setappdata(navFig, 'navWaypoints', waypoints);
    setappdata(navFig, 'navWaypointIdx', 2);
    setappdata(navFig, 'navPlannedGoalX', goalX);
    setappdata(navFig, 'navPlannedGoalY', goalY);
end

function changed = consumeNavigationGoalChanged(navFig)
    changed = false;
    if isappdata(navFig, 'navGoalChanged') && getappdata(navFig, 'navGoalChanged')
        setappdata(navFig, 'navGoalChanged', false);
        changed = true;
    end
end
