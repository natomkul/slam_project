function active = isNavigationGoalActive(navFig)
    active = isappdata(navFig, 'navGoalActive') && getappdata(navFig, 'navGoalActive');
end
