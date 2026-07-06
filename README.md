> **BIG WARNING!!!** I first pushed those files mid refactoring so there is still a lot of nonsense on it!

That being said, this version embodies the true vision I have for this little project: Have a simple almost industry grade game/rendering engine that i can later use for other projects like fluid/particle sims, raytracing, raycasting, why not help me with maths visuals for the mathematics part of my major haha.

That being said this is absolutely ambitious, and just relying on youtube, blogs and AI help won't cut it, I need some professionnal guidance! That's why i picked up Jason's Gregory Game Engine Architecture book. He is an industry titan, being the lead programmer at Naughty Dog, and the book is truly a godsend to anyone that wants to have a full understading, not only of game/graphics programming, but of the whole low-level system, and how to use that knowledge to write efficient code.

I'll keep adding stuff to this file using a log based approach. Might become really long but it's representative of the labor and time i sunk on it o7.

---

## LOG-1

Here is the current checklist of stuff that was done(X) and that needs to be done(O):

- [x] Implemented a god object that menages all the systems.
- [x] Added a fully contigous MemPool object to utilize DoP and Ecs to the fullest!
- [x] Mostly finished the Components objects refactoring.
- [x] Still have my old shaders pipeline done, which is still useful!
- [ ] Having trouble implementing the update pipeline, mainly because I'm having trouble with Cpp templates...
- [ ] Will need to rewire the Shaders a bit.
- [ ] No imGui in sight still.
- [ ] I don't know why, but i can't separate the headers and the cpp files inside two folders wihout braking the code T-T, though i think it might be a Codium quirk not necessarily a coding issue. The file tree is an absolute mess...

Still have a long ways to go.
