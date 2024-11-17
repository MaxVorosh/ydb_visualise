# Visualiser
With this tool you can visualise actors' actions based on log files.
## Build & Run
Install dependencies: 
- libjpeg8-dev
- zlib1g-dev
- libpng12-dev
- freeglut3 (needed only opengl)

Move to visualise/oxygine-framework/ydb/proj.cmake. Execute file run.sh

If you want to add new logs, change file visualise/oxygine-framework/ydb/data/logs/log.txt
## Usage
Every square represents an actor. They are divided by groups. You can navigate through the groups by pressing left and right (or A and D). 

Then, every group is combined in a group of groups and so on. Every recursion level is a layer. To navigate through layers, use up and down (or W and S). Actors of the higher (not the bottom) layers are not real. They are only represent group of actors.

When actor send a message to another actor, an arrow occures. If it pointed to/from the top corner of screen, this message going to/from the actor of upper layer. Similar to the bottom corner of the screen.

If an actor is red, then it is not registered. When it is, it becomes green.

You can pause simulation by pressing P. Also, you can see it in reverse by pressing R.