function setupNavigationGoalClick(navFig)
% setupNavigationGoalClick  Map click sets goal; no driving until first click.

    setappdata(navFig, 'navGoalActive', false);
    setappdata(navFig, 'navGoalChanged', false);
    set(navFig, 'WindowButtonDownFcn', @onFigureClick);

    function onFigureClick(fig, ~)
        navigationGoalClick(fig, get(fig, 'CurrentAxes'));
    end
end
