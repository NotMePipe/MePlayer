# MePlayer
A lightweight music player

Follow pipeline:
  - Version 1
    - SDL window
    - play single song
  - Version 2
    - FFmpeg decoding pipeline
      - It's very likely this becomes SDL3_mixer instead
      - If that's the case, SDL3/Audio needs to be replaced with SDL3_mixer
    - proper audio buffering
  - Version 3
    - metadata extraction
  - Version 4
    - library scanning
  - Version 5
    - album grouping
  - Version 6
    - UI polish
  - Version 7
    - playlists
  - Version 8
    - Discord RPC
    - Last.FM
