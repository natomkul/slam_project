function waypoints = planPath(occMap, startXY, goalXY)
% planPath  Grid A* on occupancy map; returns Nx2 world waypoints (x,y).
% Empty if no path. Falls back to straight segment if start≈goal.

    startXY = startXY(:)';
    goalXY = goalXY(:)';

    if norm(startXY - goalXY) < 0.05
        waypoints = [startXY; goalXY];
        return;
    end

    gridSize = occMap.GridSize;
    nRows = gridSize(1);
    nCols = gridSize(2);

    blocked = false(nRows, nCols);
    for row = 1:nRows
        for col = 1:nCols
            xy = grid2world(occMap, [col, row]);
            blocked(row, col) = checkOccupancy(occMap, xy) > 0.5;
        end
    end

    startGrid = world2grid(occMap, startXY);
    goalGrid = world2grid(occMap, goalXY);

    startCol = round(startGrid(1));
    startRow = round(startGrid(2));
    goalCol = round(goalGrid(1));
    goalRow = round(goalGrid(2));

    startRow = min(max(startRow, 1), nRows);
    startCol = min(max(startCol, 1), nCols);
    goalRow = min(max(goalRow, 1), nRows);
    goalCol = min(max(goalCol, 1), nCols);

    if blocked(startRow, startCol)
        blocked(startRow, startCol) = false;
    end
    if blocked(goalRow, goalCol)
        blocked(goalRow, goalCol) = false;
    end

    pathCells = astarGrid(blocked, startRow, startCol, goalRow, goalCol);
    if isempty(pathCells)
        waypoints = [];
        return;
    end

    waypoints = zeros(size(pathCells, 1), 2);
    for k = 1:size(pathCells, 1)
        xy = grid2world(occMap, [pathCells(k, 2), pathCells(k, 1)]);
        waypoints(k, :) = xy;
    end

    waypoints = subsampleWaypoints(waypoints, 0.15);
end

function path = astarGrid(blocked, startRow, startCol, goalRow, goalCol)
    [nRows, nCols] = size(blocked);

    openSet = [startRow, startCol];
    cameFrom = zeros(nRows, nCols, 2);
    cameFrom(:, :, 1) = -1;
    cameFrom(:, :, 2) = -1;

    gScore = inf(nRows, nCols);
    gScore(startRow, startCol) = 0;

    fScore = inf(nRows, nCols);
    fScore(startRow, startCol) = heuristic(startRow, startCol, goalRow, goalCol);

    closed = false(nRows, nCols);

    neighbors = [-1 0; 1 0; 0 -1; 0 1; -1 -1; -1 1; 1 -1; 1 1];

    while ~isempty(openSet)
        [~, idx] = min(arrayfun(@(i) fScore(openSet(i,1), openSet(i,2)), ...
            1:size(openSet, 1)));
        current = openSet(idx, :);
        cr = current(1);
        cc = current(2);

        if cr == goalRow && cc == goalCol
            path = reconstructPath(cameFrom, cr, cc);
            return;
        end

        openSet(idx, :) = [];

        if closed(cr, cc)
            continue;
        end
        closed(cr, cc) = true;

        for n = 1:size(neighbors, 1)
            nr = cr + neighbors(n, 1);
            nc = cc + neighbors(n, 2);
            if nr < 1 || nr > nRows || nc < 1 || nc > nCols
                continue;
            end
            if blocked(nr, nc)
                continue;
            end

            stepCost = norm(neighbors(n, :));
            tentative = gScore(cr, cc) + stepCost;

            if tentative < gScore(nr, nc)
                cameFrom(nr, nc, 1) = cr;
                cameFrom(nr, nc, 2) = cc;
                gScore(nr, nc) = tentative;
                fScore(nr, nc) = tentative + heuristic(nr, nc, goalRow, goalCol);

                if ~any(openSet(:, 1) == nr & openSet(:, 2) == nc)
                    openSet(end + 1, :) = [nr, nc]; %#ok<AGROW>
                end
            end
        end
    end

    path = [];
end

function h = heuristic(r, c, gr, gc)
    h = hypot(r - gr, c - gc);
end

function path = reconstructPath(cameFrom, row, col)
    path = [row, col];
    while true
        pr = cameFrom(row, col, 1);
        pc = cameFrom(row, col, 2);
        if pr < 0
            break;
        end
        path = [pr, pc; path]; %#ok<AGROW>
        row = pr;
        col = pc;
    end
end

function out = subsampleWaypoints(waypoints, minDist)
    if size(waypoints, 1) <= 2
        out = waypoints;
        return;
    end

    out = waypoints(1, :);
    for k = 2:size(waypoints, 1)
        if norm(waypoints(k, :) - out(end, :)) >= minDist
            out(end + 1, :) = waypoints(k, :); %#ok<AGROW>
        end
    end
    if norm(out(end, :) - waypoints(end, :)) > 1e-6
        out(end + 1, :) = waypoints(end, :);
    end
end
