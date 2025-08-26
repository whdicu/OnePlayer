# OnePlayer


#### Description
A music player with a simple and beautiful interface.


#### Function introduction
0.Support looping, shuffle, and single loops.
1.Volume control: The software can control the volume level individually, independent of the system volume.
2.Playback memory: The software starts playing the song that was playing when you last exited, and starts playing from the position you exited.
3.Cover display: Extract the picture from the song file and display it on the interface.
4.Song search: Press Ctrl F to search for the song name in the playlist.


#### Building notes
1.  QT version: 6.6.0  (Previously, when using the 6.3.2 msvc compiler, the playback lag phenomenon occurred, but the 6.3.2 mingw compiler did not have this problem).
2.  VS version: 2022  (If you use QtCreater, you should be able to develop it with a slight modification, but I haven't tried it).
3.  Another project of mine, OneDer, is also used in this project, mainly using its DList instead of QList. Pull is required and include paths configured.
4.  Use QMediaPlayer's metaData to get music images, but many times you won't be able to get them, so use FFmpeg to get music images instead.


#### Branches
1.  dev: Development branch.
2.  master: Stable branch that can be used.


#### Player Showcase
![image](./show/img1.png)
