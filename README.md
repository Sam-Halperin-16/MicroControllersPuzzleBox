# MicroControllersPuzzleBox
# Electronic Puzzle Box Design

**Status:** Design specification — revised with owner corrections  
**Date:** October 27, 2026  
**Construction:** Laser-cut wood, 3D-printed parts, a microcontroller, and electronic components

## 1. Purpose and scope

Build a six-sided physical puzzle box whose independent puzzles reveal four numbers. A player activates the box by rolling a ball into a hole, solves the puzzles while a stopwatch runs, and submits the resulting code using an infrared remote. Correct code entry stops the stopwatch and plays a short victory sound.

There is currently no prize, prize compartment, or final opening panel. Completion is an electronic success state.

This document consolidates the supplied sketches and the owner's explanations. It is the starting point for mechanical design, electronics selection, and firmware development; it is not yet a fabrication drawing or wiring specification.

### Requirement labels

- **Confirmed:** Explicitly described or clarified by the owner.
- **Working interpretation:** Derived from the sketches or prior summary and still subject to confirmation.
- **Proposed:** An implementation approach for review, not an established requirement.
- **Open:** A decision needed before the relevant implementation can be finalized.

Numbers, dimensions, part models, timing thresholds, and electrical specifications not supplied by the owner remain open. Handwritten brainstorming and crossed-out alternatives are source material, not instructions to add features.

## 2. Confirmed experience

1. The player tilts the box to roll a metal ball into a hole.
2. Ball arrival activates the box and starts a stopwatch.
3. Four puzzle paths reveal four code numbers.
4. The player can work on the paths in any order, pause one midway, and work on another.
5. The player enters the final code with an IR remote; the receiver is on the top panel.
6. One four-digit seven-segment display unit shows elapsed time as `MM:SS`, except while code entry is displayed.
7. Correct entry stops the stopwatch and plays a short victory sound.

### Independence and dependencies

The puzzles are **independent and asynchronous**, not a single ordered sequence. Each path has local dependencies, but finishing one path must not be required to start another.

For example, the wire drawer must be earned before its supplied wires can be used, but the player may leave the wire puzzle unfinished and solve the rotary lock. A global “current puzzle” variable must not restrict interaction to one path.

The labels “number 1” through “number 4” identify the four puzzle rewards, not required code positions or discovery order. **Any ordering of the four reward numbers is accepted.** Exact reward values have not been selected. The entry interface uses four decimal digits; select reward values compatible with that format. Duplicate reward values, if used, must retain their multiplicity when checking a submitted code.

Code attempts are available during active play regardless of puzzle completion, except during the explicit 30-second timeout after an incorrect submission. Walking progress has an intentional local reset: the magnet must remain in place, and removing and replacing it starts the walking route over.

## 3. Physical layout and face identification

Image 4 is roughly authoritative. The middle panel is the **top**. The surrounding faces have confirmed physical orientations:

| Face ID | Orientation | Main features |
| --- | --- | --- |
| F0 | Top | One four-digit display unit, IR receiver, five Morse LEDs, two walking cue LEDs |
| F1 | Left | Clue reference and marked magnet target with reed switch |
| F2 | Right | Potentiometer, tracing diagram, terminals, backlit number |
| F3 | Back | Rotary encoder, three combination LEDs, four direction arrows |
| F4 | Front | Magnet drawer and wire drawer |
| F5 | Bottom | No assigned puzzle function |

```text
                    +----------------------+
                    | F5: BOTTOM           |
                    | Unassigned function  |
                    +----------------------+
                    | F3: BACK             |
                    | Rotary lock          |
+-------------------+----------------------+-------------------+
| F1: LEFT          | F0: TOP              | F2: RIGHT         |
| Reference /       | Display / IR /       | Tone / wiring /   |
| magnet target     | Morse / walk LEDs    | backlight         |
+-------------------+----------------------+-------------------+
                    | F4: FRONT            |
                    | Two drawers          |
                    +----------------------+
```

This is a face-location reference based on the sketch, not a dimensioned cutting net. Actual dimensions, joinery, mounting clearances, and the relationship of each feature to folded edges remain open.

### F0 — Top panel

**Confirmed:**

- One integrated four-digit seven-segment display unit, rather than four separate display modules.
- IR receiver.
- Five independently cycling Morse LEDs spelling SHAKE.
- Two additional walking cue LEDs: one with a forward arrow and one with a rotational arrow.
- Stopwatch display in `MM:SS` during normal play, replaced by the entry buffer during code entry.

The walking LEDs provide movement instructions; they are distinct from the five SHAKE LEDs. Their exact arrangement and cue encoding remain to be designed.

The box also requires the ball activation mechanism and two capacitive touch screws, but their exact placement is not explicitly settled. The curved and forward arrow markings in the top sketch correspond to the walking cue functions; do not add a second encoder to this face.

**Mechanical design needs:** A window sized for the single display unit; IR-transparent receiver access; seven LED positions with understandable markings; accessible mounting and wiring space; isolation of intentional touch contacts from ordinary fasteners.

### F1 — Left: reference and magnet target

**Confirmed:**

- The clue-reference area is on the left face.
- The irregular shape labeled `N/S` marks the magnet placement target.
- A reed switch behind this target detects the magnet.
- The magnet must remain in place throughout the walking route.

**Working interpretation:** The clue reference includes the Morse reference shown in image 4. Its exact character set and artwork remain open.

The `N/S` marking belongs to the magnet target, not an absolute navigation requirement. Walking directions are relative and the initial orientation is irrelevant. The SHAKE LEDs themselves are on F0, not this face.

Design the target so the magnet can stay reliably positioned while the player carries and turns the box. Attachment force, panel thickness, and switch placement require physical validation.

### F2 — Tone, wiring, and backlight face

**Confirmed functions; approximate arrangement from the sketch:**

- Potentiometer for tone adjustment.
- Winding-line diagram used to trace terminal pairings.
- Electrical terminals for four required connection pairs, linked using wires obtained from a drawer.
- A hole through the wooden exterior, covered with paper, that reveals a predetermined number when backlit.
- A 3D-printed internal enclosure containing one backlight LED and blocking ambient light from seeping into the clue cavity.
- Thicker paper forming the outline of the number behind the paper cover.

The diagram is a tracing clue, not a joystick maze. A player follows its lines to discover which terminals should be linked electrically.

All four required pairs must be connected for success. The terminal layout, connector style, diagram artwork, potentiometer position, and optical dimensions remain open. Four pairs are required; the physical terminal count depends on whether the selected topology shares terminals.

### F3 — Back: rotary lock

**Confirmed:**

- A rotary encoder provides a locker-style combination interaction.
- Numbered dial positions are written into the box around the control.
- Three LEDs along the bottom of this panel provide the three combination values through blink counts.
- An arrow above each LED indicates the direction for reaching that entry.
- A fourth arrow after the final LED indicates the additional reversal needed to commit the third entry.
- Reversing direction commits the previous dial entry.
- No joystick is included.

Ignore the irregular shape drawn on the back in image 4. The magnet target and reed switch belong on the left face, F1.

Dial range, angular spacing, pointer/reference arrangement, and the encoder-to-dial-position mapping remain open.

### F4 — Front: drawer face

**Confirmed:** Two independently released drawers.

| Drawer | Contents | Release condition |
| --- | --- | --- |
| Magnet drawer | One magnet | Both designated screws are touched simultaneously |
| Wire drawer | Loose connecting wires | The potentiometer-controlled tone is matched correctly |

The words “magnet” and “wires” in the sketch identify contents; whether they appear as exterior labels is open. Drawer dimensions, travel, handles, retention, and lock mechanisms remain open.

### F5 — Bottom: unassigned function

The far panel in image 4 folds to form the bottom and is largely blank or shaded. It has no confirmed puzzle function. Do not assign it as a prize lid. Whether it provides maintenance access is an open mechanical decision.

### Shared internal parts

The speaker, vibration motor, microcontroller, power system, and lock drivers may be mounted internally. Their mounting points and any speaker grille remain open. Interior packaging must preserve the ball mechanism, drawer travel, touch sensing, and clearance behind the magnet target.

## 4. Puzzle A — Morse, shaking, and vibration

**Reward:** Code number 1.

### Visible clue

Five LEDs on the top face, F0, independently repeat Morse characters that collectively spell **SHAKE**:

| LED / letter | Morse |
| --- | --- |
| S | `...` |
| H | `....` |
| A | `.-` |
| K | `-.-` |
| E | `.` |

Each LED repeats its own character in a cycle. The cycles are randomly offset from one another; they are not a synchronized presentation of the entire word. The × marks in image 1 mean LED off.

The player observes individual LEDs, identifies each character, and reads SHAKE from their physical arrangement. A readable ordering of the five LEDs must be established in the panel artwork.

### Interaction and reward

1. Decode SHAKE.
2. Shake the box somewhat vigorously.
3. The accelerometer detects a shake distinct from walking or ordinary tilting.
4. The vibration motor communicates the first number in Morse.
5. Shake again to replay the vibration message.

### Implementation decisions

- **Proposed:** Use consistent Morse timing ratios for dots, dashes, and gaps. Exact durations require physical testing.
- **Proposed:** Choose random phase offsets when the clue starts, then preserve each LED's repeating cadence. Re-randomizing every flash would make the characters harder to interpret.
- **Confirmed:** Another qualifying shake replays the vibration message.
- **Confirmed:** Recognition must require somewhat vigorous movement and distinguish it from walking and tilting.
- **Open:** Exact thresholds, duration, cooldown, and handling of a new shake during an existing vibration message.
- **Open:** How a player knows where the five-character word begins and ends.

The controller cannot know whether the player has mentally decoded SHAKE. Gesture detection therefore does not require proof of decoding.

## 5. Puzzle B — Touch, magnet, and walked number

**Reward:** Code number 2.

### Touch and drawer release

Two designated screws use **capacitive touch sensing** and must both be touched simultaneously. Correct simultaneous contact releases the magnet drawer. Touching only one screw must not satisfy the condition. The circuit, calibration, debounce, and required overlap duration remain open.

### Magnet placement and reset

The player takes the magnet and places it at the irregular `N/S` target on the left face, F1. A reed switch detects placement and starts the walking interaction.

The magnet must remain in place during the route. Removing it interrupts the attempt; replacing it starts the walking route over from the beginning. This explicit local reset is an exception to general progress preservation across activities.

### Walking interaction

Two LEDs on the top face provide the relative walking instructions:

- A **forward-arrow LED** indicates forward movement.
- A **rotational-arrow LED** indicates a turn.

Walking instructions are visual, not spoken. The accelerometer tracks walking movement and the gyroscope tracks turning. The player follows the route, which draws the shape of a number when viewed as a path. Initial orientation is irrelevant; the route uses relative movements rather than compass headings.

When the route is completed, **both walking LEDs blink three times and then stop**. The player infers the second number from the walked shape.

### Implementation decisions

- **Open:** Exact route and target number.
- **Open:** LED encoding of step counts, turn direction, and turn angle; the two arrow symbols establish functions but do not yet specify the full signaling scheme.
- **Open:** Cue timing and advancement criteria after detected steps or turns.
- **Open:** Carrying posture, accelerometer step-detection calibration, gyroscope calibration, and turn tolerances.
- **Open:** Pause/recovery behavior while the magnet remains attached and the player changes activities.
- **Open:** Completion blink timing and behavior if the magnet is removed during completion feedback.

The firmware must distinguish actual route movements from incidental handling, shaking, and motor vibration. Gyroscope-based relative turns do not require an absolute starting heading.

## 6. Puzzle C — Tone matching, wires, and illuminated number

**Reward:** Code number 3.

### Tone matching

The potentiometer controls the adjustable tone. The target tone is randomly chosen.

Whenever the potentiometer changes and is then left stationary for a short settling period, play the **current tone based on potentiometer position, followed by the target tone**. If the current tone is within the matching threshold of the target, release the wire drawer.

A settling period of **200 ms is the owner's example**, not a fixed final tuning value. This period detects that adjustment has stopped; it is not a separate requirement to hold a matching tone for a longer duration.

**Open:** Frequency range and mapping, threshold, tone duration and separation, potentiometer noise tolerance, target selection lifetime, and behavior when the knob changes during playback. In particular, decide when a randomly selected target is refreshed so it remains learnable during an attempt.

### Diagram tracing and wire linking

The player obtains wires from the drawer, then traces the winding lines on F2 to discover which terminals to link.

There are **four required connection pairs**. All four pairs must be connected for success; satisfying only a subset must not activate the backlight. The visible diagram supplies the mapping and the physical wire connections submit the solution.

**Open:** Terminal topology and physical count, connection detection, and whether additional incorrect links invalidate otherwise correct connections.

### Hidden number and optical construction

The hidden clue uses:

1. A hole in the wooden exterior on F2.
2. Paper covering the hole.
3. Thicker paper forming the outline of a predetermined number.
4. A 3D-printed enclosure mounted inside the box behind the opening.
5. One LED inside the enclosure to backlight the paper.

The printed enclosure contains the light source and blocks ambient light from seeping into the clue cavity. When illuminated, the difference in paper thickness reveals the number. This is a paper-outline construction, rather than a requirement for ink printing to form the number.

### Implementation decisions

- **Open:** Connector types, wire lengths, and whether exactly four individual jumper wires implement the four pairs.
- **Open:** Circuit used to detect all four required links.
- **Open:** Whether the backlight stays on after solving or follows the current wiring state.
- **Open:** Paper thicknesses, outline fabrication, aperture size, LED placement, brightness, diffusion, and enclosure dimensions.
- **Proposed:** Use current-limited sensing suitable for arbitrary player connections; determine the actual circuit during electronics design.

## 7. Puzzle D — LED combination and rotary lock

**Reward:** Code number 4.

### Combination clue

The puzzle begins with its three LEDs blinking; no joystick action is required. Each LED blinks the correct count for one part of the combination. An arrow above that LED shows which direction to turn.

The physical arrangement must communicate the order of the three entries. Exact counts, blink timing, pauses, and repeat behavior remain open.

### Rotary interaction

The blink counts refer to **numbered dial positions written into the box**, not relative encoder-detent counts. The player turns the dial in each indicated direction to reach the corresponding numbered position.

A direction reversal commits the preceding entry:

1. Follow the first arrow to the position given by LED 1.
2. Reverse direction, committing entry 1, then reach the position given by LED 2.
3. Reverse direction, committing entry 2, then reach the position given by LED 3.
4. Reverse once more, following the fourth arrow after the final LED, to commit entry 3.

No separate submit button is required for this interaction. The fourth arrow supplies a commit gesture, not a fourth combination value or fourth combination LED.

**Working interpretation from the original solve notes:** Correct completion causes the speaker to speak the fourth number.

### Implementation decisions

- **Open:** Dial range, numbering, position mapping, and initial reference/homing behavior.
- **Open:** Position tolerance, full-turn rules, overshoot rules, and filtering of accidental small reversals.
- **Open:** Reset gesture and feedback for an incorrect combination.
- **Open:** Replay method for the spoken reward.

An encoder push-button is not required by the confirmed input method.

## 8. Stopwatch, IR entry, and completion

### Stopwatch

- Start timing when the ball reaches the activation hole.
- Show elapsed time as **`MM:SS`** on one integrated four-digit seven-segment display unit during normal play.
- Temporarily replace the time with the code-entry buffer during IR entry.
- Stop the stopwatch only after a correct code is submitted.
- Overflow behavior is undefined and is outside the current design scope.

**Proposed:** The stopwatch continues internally during code entry and incorrect-code timeout; neither pauses the timed solve.

**Open:** Separator implementation and presentation of the final time after victory.

### IR entry buffer and submission

The receiver is mounted on F0. The player uses an IR remote to enter four numbers and presses **OK** to submit.

- Numeric presses update a four-position entry buffer.
- Further numbers cyclically overwrite earlier positions; filling the buffer does not submit it.
- Only **OK** submits the current code for validation.
- Any ordering of the four puzzle reward numbers is accepted.
- Code attempts are allowed regardless of puzzle completion, subject only to the incorrect-entry timeout during active play.

**Proposed buffer behavior:** Fill the four displayed positions left to right, then wrap the write cursor to the first position and continue cyclically. For example, pressing `1 2 3 4 5` displays `5234`, with the next press replacing the second position. This illustrates cyclic overwrite; the exact display/cursor treatment remains open.

**Proposed validation:** Compare the submitted digits with the four reward digits as multisets, preserving duplicate counts. Permutation acceptance does not allow a missing reward digit to be replaced by a duplicate of another.

### Incorrect-code timeout

Every incorrect submitted code starts a **30-second timeout during which no code can be entered**. Do not treat it as a timeout of the entire puzzle box: the independent puzzles remain available.

**Proposed:** Ignore numeric and OK entry events during the timeout, retain the session timer, and restore entry eligibility when 30 seconds have elapsed. Do not extend the timeout because the player presses remote keys during it.

**Open:** Timeout display/feedback, whether to clear or retain the buffer afterward, incomplete-buffer handling on OK, clear/backspace controls if any, entry-mode exit behavior, remote model/protocol, and handling of repeated IR frames.

### Victory

A correct submitted code stops the stopwatch and plays a short victory sound. No final mechanical unlock is required.

**Open:** Post-victory display, whether puzzle outputs remain available, and how a new session is started.

## 9. Electronics and materials inventory

The later handwritten bill of materials is the baseline inventory. It does not yet cover every supporting part required for a working build.

| Item | Baseline quantity | Function / status |
| --- | ---: | --- |
| Microcontroller | 1 | Raspberry Pi Pico 1, using the RP2040; pin allocation and firmware environment open |
| Reed switch | 1 | Magnet detection |
| Magnet | 1 | Tool in magnet drawer |
| Speaker | 1 | Tone comparison, spoken rotary reward, and victory sound; no walking instructions |
| Rotary encoder | 1 | Locker-style combination |
| Potentiometer | 1 | Adjustable tone |
| Backlight assembly | 1 | One LED in a 3D-printed light-blocking enclosure behind the paper-outline clue |
| Drawer locks | 2 | Independent magnet and wire drawer releases; actuator type open |
| Four-digit seven-segment display unit | 1 | Four digits in one module; stopwatch and code entry |
| LEDs | Approximately 12 | Five top Morse LEDs, two top walking LEDs, three back combination LEDs, one backlight LED: 11 assigned; remaining allowance open |
| Accelerometer | 1 sensing function | Shaking and walking movement; may share an IMU module with the gyroscope |
| Gyroscope | 1 sensing function | Relative turn tracking; may share an IMU module with the accelerometer |
| Offset vibration motor | 1 | Vibration Morse output |
| Metal ball | 1 | Activation mechanism |
| Ball detection mechanism | TBD | Detect arrival at the activation hole |
| Touch screws | 2 active contacts | Simultaneous-touch interaction; possible decorative screws open |
| Capacitive touch sensing electronics | TBD | Detect simultaneous touch on both screws |
| IR receiver and remote | 1 each | Final code entry |
| Loose wires and terminals | TBD | Player-operated connection puzzle |
| Paper cover and thicker paper number outline | 1 clue assembly | Predetermined number revealed by backlighting |
| Wood panels | 6 faces plus internal parts | Laser-cut enclosure |
| 3D-printed parts | TBD | Mechanisms and mounts as needed |
| Power source, regulation, and switch | TBD | Supply electronics and actuators |
| Drivers, protection, wiring, connectors, fasteners | TBD | Supporting integration parts |

A keypad, joystick, additional encoders, and a prize lock are not current requirements. Early spring-lock and actuator ideas do not establish a specific lock design. The approximately 12 LEDs are separate from the integrated four-digit display unit. Eleven discrete LEDs now have assigned roles, counting the single backlight LED; approximately 12 remains the sketch estimate, not a requirement to add another function.

## 10. Proposed firmware structure

The hardware platform is the RP2040-based Raspberry Pi Pico 1. This section proposes firmware structure; language, SDK, libraries, peripheral modules, and pin allocation remain open.

### Session state

Use a small session lifecycle:

```text
READY --ball detected--> ACTIVE --correct IR code--> COMPLETE
```

Reset and maintenance transitions remain to be designed. “READY” may mean electrically powered and waiting; whether the ball physically powers the system or triggers a powered controller is open.

### Independent puzzle state

Maintain separate state for each path:

| Path | Example state to retain |
| --- | --- |
| Morse / shake | LED phase schedules, shake recognition, vibration playback position |
| Touch / walking | Capacitive contact states, drawer release, reed state, visual route cue, counted steps, relative turn, completion blinks; reset route on magnet replacement |
| Tone / wires | Potentiometer change/settling time, random target, tone playback, drawer release, four pair readings, backlight status |
| Rotary | Numbered dial position, direction, three reversal-committed entries, final commit reversal, completion |
| IR entry | Four-position buffer, cyclic write cursor, filled positions, OK submission, timeout deadline |

Use nonblocking timing so LED flashing, encoder input, touch sensing, IR reception, stopwatch updates, and sensor sampling continue during other activities.

### Shared resource coordination

- **Speaker:** Tone comparison, the spoken rotary reward, and victory audio share one output. Define an arbitration and replay policy so one interaction does not destroy another's progress.
- **Accelerometer and gyroscope:** Shake, walking, and turn detection share motion observations. Distinguish vigorous shakes from walking and tilting; do not let shaking count as route progress. Relative turns use the gyroscope.
- **Walking cues:** Maintain the forward and rotation LED cues without blocking the independently scheduled SHAKE LEDs on the same face. Magnet removal interrupts the route; replacement resets it.
- **Vibration motor:** Its movement may affect accelerometer readings. Validate isolation or filtering while it runs.
- **Display and timeout:** Time and code entry share one four-digit unit. Keep the entry timeout separate from puzzle state and stopwatch accounting; it must not block sensor polling or other interactions.
- **Power:** Drawer actuation, audio, and the backlight may overlap. Determine supply capacity using selected hardware.

Asynchronous puzzle progress does not require simultaneous audio playback. It does require that shared output scheduling preserves progress and lets the player resume an interrupted interaction.

### Configuration

Keep puzzle values and timing parameters configurable: four reward numbers, vibration message, visual walking route, randomly selected tone target and its lifetime, tone settling period (200 ms example) and tolerance, four terminal pairs, numbered rotary combination, Morse timing, sensor thresholds, and audio assets. The confirmed incorrect-code timeout is 30 seconds; validation accepts any permutation of the reward digits.

## 11. Mechanical design work

Before producing laser-cut or printed parts, resolve:

1. Overall box dimensions and panel thickness.
2. Wood species, kerf allowance, joinery, and assembly method.
3. Internal division between drawers, electronics, and the ball mechanism.
4. Ball path, target hole, retention, and reset method.
5. Drawer locks, release travel, retention after release, and reset access.
6. Sensor and control mounting details.
7. Left-face reed-switch distance through the panel and magnet attachment that remains stable during walking and turns.
8. Touch-contact isolation from surrounding hardware.
9. A printed internal enclosure containing one LED, blocking light seepage, and aligning with the paper-covered wooden aperture and thicker paper number outline.
10. Service access for power, wiring, and mechanical recovery.

**Proposed material allocation:** Laser-cut wood for the exterior and structural partitions; printed parts for custom brackets, guides, drawer details, and ball-path components where useful. Final allocation depends on tolerances and prototype results.

## 12. Validation plan

These are acceptance checks for prototypes, not claims that testing has already occurred.

| Area | Acceptance check |
| --- | --- |
| Activation | Ball arrival starts one session and the stopwatch; subsequent ball movement does not restart an active run |
| Stopwatch | Time remains accurate while audio, LEDs, code entry, and locks are active |
| SHAKE LEDs | Each character is independently readable over repeated cycles despite phase offsets |
| Shake reward | A somewhat vigorous shake plays interpretable vibration Morse; shaking again replays it; walking and ordinary tilting do not trigger it |
| Touch | Either capacitive screw alone fails; simultaneous capacitive contact releases the magnet drawer |
| Magnet | Placement at the left-face target starts the walking interaction; removal interrupts it and replacement starts the route over |
| Walking | Top forward/rotation LEDs guide a route using accelerometer steps and gyroscope turns, with no spoken instructions or absolute starting orientation; both LEDs blink three times and stop on completion |
| Tone | Changing then settling the pot plays current then random target tone; a within-threshold match releases the wire drawer |
| Wiring | All four required pairs activate the backlight; each incomplete subset fails; extra links follow the chosen rule |
| Hidden paper number | One enclosed LED reveals the thicker paper number outline through the paper-covered aperture; enclosure blocks light seepage |
| Rotary | Three blink counts specify numbered positions; reversals commit entries; the fourth arrow guides the reversal that commits the third entry |
| IR | Cyclic numeric overwrite continues without submission until OK; every permutation of the reward digits is accepted, preserving duplicates; attempts require no puzzle-completion flags |
| Incorrect entry | An incorrect submission blocks entry for 30 seconds while other puzzles remain usable; entry becomes available afterward |
| Victory | Correct submission stops time and plays the victory sound |
| Asynchronous play | Pause each path partway and resume without unintended loss; removing/replacing the magnet intentionally resets the walking route |
| Shared resources | Audio transitions and motor/sensor interactions do not corrupt another path |
| Reset | Once designed, reset restores mechanisms, supplied tools, and firmware for another complete run |

## 13. Open decision register

| ID | Decision | Needed for |
| --- | --- | --- |
| O01 | Box dimensions, materials, joinery, and service access | Mechanical CAD |
| O02 | Placement of activation and touch screws; arrangement of confirmed top LEDs; remaining reference artwork | Panel artwork |
| O03 | Ball sensing, retention, and physical reset | Startup mechanism |
| O04 | Four reward digit values; any permutation is accepted | Puzzle content |
| O05 | Morse timings, shake thresholds, and handling of shakes during playback | Puzzle A |
| O06 | Capacitive sensing circuit, calibration, and drawer actuator selection | Drawer mechanisms |
| O07 | Walking route, LED cue encoding, carrying posture, motion calibration, and pause with magnet retained | Puzzle B |
| O08 | Tone range, threshold, playback timing, settling interval, random target lifetime, and audio arbitration | Puzzle C and shared speaker |
| O09 | Terminal map, connectors, sensing, and extra-link rules | Wiring puzzle |
| O10 | Paper thicknesses, LED/enclosure dimensions, diffusion, and solved-light behavior | Hidden clue |
| O11 | Numbered dial mapping, reference position, reversal tolerance, error/reset rules | Puzzle D |
| O12 | IR protocol, partial-buffer handling, display/exit behavior, and timeout feedback/buffer retention | Final entry |
| O13 | Pico 1 pin budget, IMU/display modules, capacitive sensing, power, audio hardware, and drivers | Electronics schematic |
| O14 | Session reset, persistence across power loss, and post-victory behavior | Firmware lifecycle |

## 14. Source sketches

The original attachments remain at the following local paths. These links are references; the source images have not been copied into the repository.

| Image | Contents | Source |
| --- | --- | --- |
| 1 | SHAKE Morse timing; × marks indicate off | [Original sketch](/Users/micah/Downloads/rn_image_picker_lib_temp_5ccb3a29-6221-4dfb-a4a1-0c717cad5d6e.jpg) |
| 2 | Early components, ideas, and preliminary face layout | [Original sketch](/Users/micah/Downloads/rn_image_picker_lib_temp_c7053958-8890-4ad7-a986-67dde221e9db.jpg) |
| 3 | Initial solve paths and mechanism notes | [Original sketch](/Users/micah/Downloads/rn_image_picker_lib_temp_edf2b9bc-3589-414c-9326-afb3fe138fce.jpg) |
| 4 | Roughly authoritative six-face layout | [Original sketch](/Users/micah/Downloads/rn_image_picker_lib_temp_1045044e-a759-4832-8956-2e89bfd3bae7.jpg) |
| 5 | Later handwritten bill of materials | [Original sketch](/Users/micah/Downloads/rn_image_picker_lib_temp_2b9acde1-ecd0-4ab3-bae9-353917c66225.jpg) |

Owner clarifications take precedence over ambiguous or earlier sketch ideas. The current specification incorporates confirmed face orientations; the left-face magnet target; top Morse and visual walking LEDs; an integrated four-digit display; capacitive touch; accelerometer and gyroscope motion sensing; repeat-on-shake vibration; magnet-dependent walking reset; three completion blinks; settled-pot tone comparison; four required connection pairs; the enclosed single-LED paper clue; reversal-committed numbered dial entries with a fourth arrow; permutation-tolerant cyclic IR entry with OK submission and a 30-second incorrect-code timeout; and the RP2040-based Pico 1. There is no joystick or prize panel.
