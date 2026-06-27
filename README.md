So this is the my first attempt at graphical/game programming.

For learning how this type of program works i mainly used youtube videos (shoutout to pikuma and GraverDev
whom both made absolutely incredible introductory series for everything game/graphic programming).

Of course, I also need to stress how usefull AI has been in all facets of my on going journey: It is an incredible
time saver and source for sometimes really difficult to find (and mostly understand) documentation.

Now, as for the key features:

---First is of course the rasterizer that I was able to make. This one uses the Breseham algorithm to make the edges,
and the scanline to fill the shapes.

---Second, we have of course the really nice icosphere generator. It really awsome to see how spheres can be
approximated in a digital environment. I am particularly proud of how the recurssion turned out.

---The height map is also pretty fun to look at, and actually i think actually using the same map and only switching
formulas was actuall quite clever to save memory. The formula abstract class was also pretty cool.

One thing that is made clear from this first version though, and will be a problem on the future ones too, is the
scene/input management. For the most part I just improvised it. Even though it works, trying to make a fully fledge engine
on those bones does not pass at all.
