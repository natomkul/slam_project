clear; clc;

%% Parameters
L = 0.165;      % wheel distance [m]
Q = diag([1e-4, 1e-4, 1e-3, 1e-2]);
R_lidar = diag([0.05^2, 0.05^2, deg2rad(5)^2]);

%% LiDAR parameters
maxRange = 5;

%% EKF state: [x; y; angle; v]
x_est = [0; 0; pi/2; 0];
P = diag([1, 1, 0.5, 1]);

%% Occupancy map
mapResolution = 20;
mapWidth = 8;
mapHeight = 8;
mapOrigin = [-4 -4];

occMap = occupancyMap(mapWidth, mapHeight, mapResolution);
occMap.GridLocationInWorld = mapOrigin;

scanHistory = {};


%% Pose graph
pg = poseGraph;

prevScan = [];
prevPoseEKF = x_est(1:3)';
nodeId = 1;

infoMat = [100 0 0 100 0 50];

%% Histories
ekf_hist = [];
lidar_hist = [];
pg_hist = [];


tcpObj = tcpserver("127.0.0.1", 5005);

while true

    if tcpObj.NumBytesAvailable <= 0
        pause(0.01);
        continue;
    end

    line = readline(tcpObj);

    if strcmp(strtrim(line), "STOP")
        disp("Received STOP command");
        break;
    end

    dataRead = str2double(split(strtrim(line), ","));

    if numel(dataRead) < 8 || any(isnan(dataRead(1:8)))
        disp("Bad TCP packet header");
        continue;
    end
    
    %packet: [en_dt, distanceR, distanceL, imu_dt, gyro, accelerationX,
    %           accelerationY, lidar_n, (lidar_dist_n)x n, (lidar_ang_n)x n]
    en_dt = dataRead(1);
    distanceR = dataRead(2); 
    distanceL = dataRead(3);
    imu_dt = dataRead(4);
    gyro = dataRead(5);
    accelerationX = dataRead(6);
    accelerationY = dataRead(7);
    lidar_n = dataRead(8);

    en_dt = en_dt / 1000000000;
    imu_dt = imu_dt / 1000000000;

    expectedLen = 8 + 2*lidar_n;

    if numel(dataRead) < expectedLen || any(isnan(dataRead(1:expectedLen)))
        disp("Bad TCP packet");
        continue;
    end

    lidar_dist = dataRead(9: 8 + lidar_n)';
    lidar_ang = dataRead(9 + lidar_n: 8 + lidar_n * 2)';

    lidar_dist(lidar_dist <= 0) = maxRange;
    lidar_dist(lidar_dist > maxRange) = maxRange;
    lidar_ang = wrapToPi(-deg2rad(lidar_ang));
    
    scan = lidarScan(lidar_dist, lidar_ang);

    %% ================= EKF =================
    u = [distanceR; distanceL; gyro; accelerationX; accelerationY];

    [x_pred, F] = estimateRobotState(x_est, u, en_dt, imu_dt, L);
    P_pred = F * P * F' + Q;


    x_est = x_pred;
    P = P_pred;

    ekfPose = x_est(1:3)';

   %% ============ FIRST SCAN INITIALIZATION ============
    if isempty(prevScan)
        prevScan = scan;
        prevPoseEKF = ekfPose;
        scanHistory{1} = scan;

        ekf_hist(:, end+1) = x_est(1:2);
        drawMap(occMap, ekf_hist, pg_hist, x_est);
        continue;
    end

    %% ============ LiDAR SCAN MATCHING ============
    ekfDelta = posisionDifference(prevPoseEKF, ekfPose);

    useScanMatch = true;

    try
        [scanDelta, stats] = matchScans(scan, prevScan, ...
            "InitialPose", ekfDelta, ...
            "MaxIterations", 50);

        if any(isnan(scanDelta)) || ...
           norm(scanDelta(1:2) - ekfDelta(1:2)) > 0.05 || ...
           abs(wrapToPi(scanDelta(3) - ekfDelta(3))) > deg2rad(5)
            useScanMatch = false;
        end

    catch
        useScanMatch = false;
    end

    %relPose = ekfDelta;
    %useScanMatch = false;

    if useScanMatch
        relPose = scanDelta;
    else
        relPose = ekfDelta;
    end

    %% ================= EKF UPDATE FROM LiDAR =================
    if useScanMatch
        lidarPose = moveRobot(prevPoseEKF, scanDelta);

        z = lidarPose(:);

        H = [
            1 0 0 0;
            0 1 0 0;
            0 0 1 0
        ];

        innovation = z - H * x_est;
        innovation(3) = wrapToPi(innovation(3));

        K = P * H' / (H * P * H' + R_lidar);

        x_est = x_est + K * innovation;
        x_est(3) = wrapToPi(x_est(3));

        P = (eye(4) - K * H) * P;
    end

    ekfPose = x_est(1:3)';
    ekf_hist(:, end+1) = x_est(1:2);

    %% ============ POSE GRAPH SLAM ============
    addRelativePose(pg, relPose, infoMat);
    nodeId = nodeId + 1;

    scanHistory{nodeId} = scan;

    if mod(nodeId, 10) == 0
        pg = optimizePoseGraph(pg);
    end

    pgPosesLocal = nodes(pg);

    initialPose = [0 0 pi/2];
    pgPosesWorld = zeros(size(pgPosesLocal));

    for i = 1:size(pgPosesLocal, 1)
        pgPosesWorld(i, :) = moveRobot(initialPose, pgPosesLocal(i, :));
    end

    pg_hist = pgPosesWorld(:, 1:2)';
    
   %% ============ OCCUPANCY MAP REBUILD ============
    occMap = occupancyMap(mapWidth, mapHeight, mapResolution);
    occMap.GridLocationInWorld = mapOrigin;

    numMapScans = min(size(pgPosesWorld, 1), numel(scanHistory));

    for i = 1:numMapScans
        thisScan = scanHistory{i};

        if isempty(thisScan)
            continue;
        end

        insertRay(occMap, pgPosesWorld(i, :), ...
            thisScan.Ranges, thisScan.Angles, maxRange);
    end

    prevScan = scan;
    prevPoseEKF = ekfPose;

    if mod(nodeId, 10) == 0
        drawMap(occMap, ekf_hist, pg_hist, x_est);
    end
end

clear tcpObj;