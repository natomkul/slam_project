function new_posision = moveRobot(posision1, posision2)

    x1 = posision1(1);
    y1 = posision1(2);
    a1 = posision1(3);

    x2 = posision2(1);
    y2 = posision2(2);
    a2 = posision2(3);

    R = [cos(a1) -sin(a1);
         sin(a1)  cos(a1)];

    p = [x1; y1] + R * [x2; y2];

    new_posision = [p(1), p(2), wrapToPi(a1 + a2)];
    
end