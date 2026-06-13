function [goalX, goalY, showGoal] = readNavigationGoal(navFig, x_est)
% readNavigationGoal  Goal for display; showGoal false until user clicked map.

    showGoal = isNavigationGoalActive(navFig);

    if showGoal
        goalX = getappdata(navFig, 'navGoalX');
        goalY = getappdata(navFig, 'navGoalY');
    else
        goalX = x_est(1);
        goalY = x_est(2);
    end
end
