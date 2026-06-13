function waypoints = getNavigationWaypoints(navFig)
    waypoints = [];
    if isappdata(navFig, 'navWaypoints')
        waypoints = getappdata(navFig, 'navWaypoints');
    end
end
