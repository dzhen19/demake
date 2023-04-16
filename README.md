Demake: Overcooked
=====

<img width="404" alt="Screen Shot 2023-04-16 at 6 27 59 PM" src="https://user-images.githubusercontent.com/55254786/232346174-74f327f3-1e15-49fa-ab6a-f2bf3741241f.png">

We chose to demake Overcooked, which is a cooking / party game developed by indie Studio Ghost Town games. In Overcooked, you play as chefs in a kitchen. The goal is to fulfill as many orders as possible within the alotted time. Levels are designed such that players have to cooperate and communicate in order to work in the most efficient manner. Overcooked launched in 2016, so the game has many mechanics and features that are pretty difficult to replicate on the NES. We identified the following mechanics to include in our demake, which are most important to the core game loop: 

- Multiplayer cooperative gameplay
- Recipes the involve preparation and combination of ingredients
- Cute aesthetic

<img width="404" alt="Screen Shot 2023-04-16 at 6 27 59 PM" src="https://user-images.githubusercontent.com/55254786/232346154-5d6d3f80-c95e-4186-b47d-761612f371fc.png">

<h1>Our Version</h1>

We were able to preserve most of the core mechanics of the original game in our demake. In our version, players can move around, grab ingredients from crates, set ingredients down on counters, prepare ingredients using the cutting board / stove, combine ingredients into dishes, and serve dishes. Both players can interact with the same ingredients on the map, allowing for cooperative gameplay. There is also a score counter based on how many dishes you deliver and a time limit. This is pretty much the core game loop of the original game. 

Unfortunately, we were not able to implement the namesake feature. If you leave the rice on the stove for too long, in the original game, the pot will burn (overcook) and become unusable. We also did not implement throwing, dashing, and the order queue from the original. There is also only one level and one recipe in our game. 

<img width="200" alt="sprites" src="https://user-images.githubusercontent.com/55254786/232347022-d1fcff32-b371-4143-a54f-6cbdc30bf219.png">

<h1>Technical Challenges</h1>

<b>Sprites: </b> The main limiting factor for us was sprites. OverCooked is a very sprite-heavy game. Not only did we need sprites for all the ingredients, but also sprites for all the intermediate stages ingredients could be in (ie. chopped fish, steamed rice, fish and rice, etc.) This meant that we could do one or two recipes top for our demake, or else use a very convoluted polygon based pattern table. We also used the ppu to render sprites. Since each of our ingredients was a 16x16 metasprite, this meant that we could at most display only 14 ingredients on the screen at any given time (we need 2 metasprites for the players). 

<img width="200" alt="sprites" src="https://user-images.githubusercontent.com/55254786/232347200-9cd9de95-b4c1-4e99-abd9-e8002ec99b98.png">

<b>Object Map: </b> Because OverCooked is a game where the player interacts frequently with the map, we also had to keep track of where everything was on the map at any given time. We kept a 16x15 array that stored the IDs of all the tiles, and a separate 16x15 array that stored the IDs of all the items on those tiles. We would have to constantly convert the player's position from pixels (256x224px) to this sprite grid (16x15) in order to do collision detection.

<b>Custom Pattern tables / Nametables: </b> In order to draw the maps, we had to find tools to convert pngs (which we could edit in a pixel art editor) to .chr and .nam files that 8bitworkshop would recognize. You can see my edstem post on this for more detail: https://edstem.org/us/courses/34937/discussion/2871332






