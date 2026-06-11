function pointsOut = transformPoints2D(points, pose)
    x = pose(1);
    y = pose(2);
    th = pose(3);

    R = [
        cos(th), -sin(th);
        sin(th),  cos(th)
    ];

    pointsOut = points * R' + [x, y];
end