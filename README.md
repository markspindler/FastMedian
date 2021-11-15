# FastMedian
FastMedian is a plugin for the compositing software Nuke by Foundry that performs the same filter as Nuke's own Median node, but in a much shorter render time. Depending on the image content, it will usually perform 6-9x faster. On certain images, the speed increase might be even more than 20x.

Furthermore, it seems to run more stable than Nuke's Median, which might freeze Nuke if the radius is set too high. This has never happend to me with FastMedian so far.