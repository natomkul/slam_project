function publishNavigationStep(cmdClient, x_est, goalX, goalY, navParams)
% Compute move/rotate from pose and goal; send one line to C++ if non-empty.

    cmdLine = computeNavigationRequest( ...
        x_est(1), x_est(2), x_est(3), goalX, goalY, navParams);

    if strlength(string(cmdLine)) == 0
        return;
    end

    sendNavCommand(cmdClient, cmdLine);
end
