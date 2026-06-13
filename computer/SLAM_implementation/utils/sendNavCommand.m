function ok = sendNavCommand(client, line)
% sendNavCommand  Send one nav command line to C++ (e.g. "move,0.10" or "rotate,0.35").
% client from openNavCommandClient; no-op if client is empty.

    ok = false;

    if isempty(client)
        return;
    end

    line = strtrim(string(line));
    if strlength(line) == 0
        return;
    end

    writeline(client, line);
    ok = true;
end
