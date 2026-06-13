function navigationGoalClick(navFig, ax)
% navigationGoalClick  Set nav goal from map click (world x,y in meters).

    if isempty(navFig) || ~ishandle(navFig) || isempty(ax) || ~ishandle(ax)
        return;
    end

    pt = ax.CurrentPoint;
    gx = pt(1, 1);
    gy = pt(1, 2);

    setappdata(navFig, 'navGoalX', gx);
    setappdata(navFig, 'navGoalY', gy);
    setappdata(navFig, 'navGoalActive', true);
    setappdata(navFig, 'navGoalChanged', true);

    if isappdata(navFig, 'navPlannedGoalX')
        rmappdata(navFig, 'navPlannedGoalX');
    end
    if isappdata(navFig, 'navWaypoints')
        rmappdata(navFig, 'navWaypoints');
    end

    fprintf("Navigation goal set: x=%.3f y=%.3f\n", gx, gy);
end
