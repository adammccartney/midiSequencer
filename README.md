## midi Sequencer v0.1.1

A simple openFrameworks project that generates a four note sequence given a
midi note in. 
+ Harmonies in the sequence can be controlled via modename and root selection
+ Rhythmic proportions controlled via position of the HarmonicFields along the
  x axis
+ Probability of a specific step controlled by position along the y axis

![gui screenshot](img/guiScreenshot.png)

## Compilation 

```
cd $OFROOT/apps/myApps/
git clone git@github.com:adammccartney/midiSequencer.git
cd midiSequencer 
make
make RunRelease
```

## Usage

![function diagram](img/functionDiagram.png)

It's intended to be used as a single component in a modular toolchain. midi
Sequencer receives notes from a midi port and communicates the generated
sequencer over OSC to some other program (chuck/supercollider/puredata), 
which then might do something useful (like make some control messages 
for an instrument). 



### Pitch Classes

*enharmonics discounted out of love for the robots*

```
/[a-g][s]?/
```

example:
cs := c sharp 

12 pitch classes in total: {c, cs, d, ds, e, f, fs, g, gs, a, as, b}

### Modes

**legend for intervals**
```
/[M|m|P][digit]/

M       := major
m       := minor
P       := perfect
[digit] := diatonic interval
```

example: 
M2 = major second   = 2 semitones
m3 = minor third    = 3 semitones
P4 = perfect fourth = 5 semitones 

| name       | interval structure    |
|------------|-----------------------|
| major      | M2 M2 m2 M2 M2 M2 m2  |
| minor      | M2 m2 M2 M2 m2 M2 M2  |
| hminor     | M2 m2 M2 M2 m2 m3 m2  | 
| dorian     | M2 m2 M2 M2 M2 m2 M2  |
| phrygian   | m2 M2 M2 M2 m2 M2 M2  |
| lydian     | M2 M2 M2 m2 M2 M2 m2  |
| mixolydian | M2 M2 m2 M2 M2 m2 M2  |
| locrian    | m2 M2 M2 M2 m2 M2 M2  |
| maj7       | M3 m3 M3              |
| dom7       | M3 m3 m3              |
| maj6       | M3 P4 m3              |
| min6       | m3 P4 m3              |
| six4       | P4 M3 m3              |
| minsix4    | P4 m3 M3              |
| six5       | m3 m3 M2 M3           |
| minsix5    | M3 m3 M2 M3           |
| five4      | P4 M2                 |
| four2      | M2 m3                 |

