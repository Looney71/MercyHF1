# INDEX-3 Moving Walls Setup Guide

## Overview
INDEX-3 implements spatial horror mechanics with moving walls, corridor changes, and psychological disorientation effects. This guide provides step-by-step instructions for setting up the level in Unreal Engine.

## Controller Setup

### 1. Place the Controller Actor
1. In your INDEX-3 level, place an **Index3MovingWallsController** actor
2. Suggested name: `Index3Controller`
3. Position: Place centrally in the level for easy access
4. The controller will automatically find and cache all tagged actors on BeginPlay

### 2. Controller Configuration
The controller exposes these Blueprint-editable properties:

#### Timing Settings (Category: Mercy Maze|Timing)
- **Initial Delay**: 3.0s - Delay before sequence starts
- **First Wall Move Delay**: 5.0s - When first walls move
- **Corridor Change Delay**: 8.0s - When corridor reconfigures
- **Disorientation 1 Delay**: 12.0s - First disorientation phase
- **Disorientation 2 Delay**: 18.0s - Second disorientation phase
- **Final Reveal Delay**: 25.0s - Final sequence completion

#### Wall Movement Settings (Category: Mercy Maze|Walls)
- **Movement Pattern**: Sequential/Random/Synchronized/PlayerTriggered
- **Movement Type**: Slide/Rotate/Scale/Disappear
- **Wall Move Speed**: 2.0 units/second
- **Wall Move Distance**: 400.0 units

#### Audio Settings (Category: Mercy Maze|Audio)
- **Wall Move Sound**: Sound asset for wall movements
- **Corridor Change Sound**: Sound for corridor reconfigurations
- **Disorientation Sound**: Sound for disorientation effects
- **Wall Move Sound Path**: Fallback asset path

#### System Messages (Category: Mercy Maze|Messages)
- **Initial Message**: "SPATIAL RECONFIGURATION INITIATED"
- **Corridor Change Message**: "CORRIDOR LAYOUT MODIFIED"
- **Disorientation Message**: "PERCEPTION ADJUSTMENT IN PROGRESS"
- **Final Message**: "SPATIAL MAZE PROTOCOL COMPLETE"

## Required Level Actors

### 1. Wall Groups
Create wall actors and tag them appropriately:

#### Left Walls
- **Tag**: `INDEX3_LEFT_WALLS`
- **Suggested Names**: `LeftWall_01`, `LeftWall_02`, etc.
- **Purpose**: Move inward to create claustrophobia
- **Placement**: Left side of main corridor
- **Quantity**: 3-6 wall segments

#### Right Walls  
- **Tag**: `INDEX3_RIGHT_WALLS`
- **Suggested Names**: `RightWall_01`, `RightWall_02`, etc.
- **Purpose**: Move to create new passages
- **Placement**: Right side of main corridor
- **Quantity**: 3-6 wall segments

#### Corridor Walls
- **Tag**: `INDEX3_CORRIDOR_WALLS`
- **Suggested Names**: `CorridorWall_01`, `CorridorWall_02`, etc.
- **Purpose**: Rotate to change path direction
- **Placement**: Junction points and corridor intersections
- **Quantity**: 2-4 rotating segments

#### Fake Walls
- **Tag**: `INDEX3_FAKE_WALLS`
- **Suggested Names**: `FakeWall_01`, `FakeWall_02`, etc.
- **Purpose**: Appear/disappear to confuse player
- **Placement**: False passages and dead ends
- **Quantity**: 2-5 deceptive walls
- **Initial State**: Hidden (will be revealed during sequence)

### 2. Lighting System

#### Path Lights
- **Tag**: `INDEX3_PATH_LIGHTS`
- **Suggested Names**: `PathLight_01`, `PathLight_02`, etc.
- **Purpose**: Guide player, dimmed during disorientation
- **Light Type**: Point Light or Spot Light
- **Initial Intensity**: 200.0
- **Placement**: Along main path and key decision points
- **Quantity**: 4-8 lights

### 3. System Text Display

#### System Text Actor
- **Tag**: `SYSTEM_TEXT`
- **Suggested Name**: `SystemText_Index3`
- **Purpose**: Display sequence messages to player
- **Component**: TextRenderComponent or UMG Widget
- **Placement**: Visible location (wall or floating)
- **Font Size**: Large enough to read from player distance

## Level Layout Recommendations

### Basic Corridor Structure
```
[Start] ──→ [Left Walls] ──→ [Junction] ──→ [Right Walls] ──→ [End]
                ↓              ↓              ↓
           [Path Lights]  [Corridor Walls] [Fake Walls]
```

### Suggested Dimensions
- **Main Corridor Width**: 400-600 units
- **Wall Height**: 300-500 units
- **Wall Move Distance**: 400 units (configurable)
- **Junction Size**: 800x800 units minimum

## Sequence Behavior

### Phase 1: Initial (3s delay)
- System message appears
- All timers set for upcoming events

### Phase 2: First Wall Move (5s)
- Left walls move inward (claustrophobia effect)
- Wall movement sound plays
- Corridor narrows

### Phase 3: Corridor Change (8s)
- Corridor walls rotate 90 degrees
- Right walls move outward
- New passages open
- "CORRIDOR LAYOUT MODIFIED" message

### Phase 4: Disorientation 1 (12s)
- Fake walls become visible
- Path lights dim to 50% intensity
- "PERCEPTION ADJUSTMENT IN PROGRESS" message
- Player confusion increases

### Phase 5: Disorientation 2 (18s)
- Left walls scale up (1.5x height)
- Right walls scale down (0.7x height)
- Fake walls hide again
- Size distortion effect

### Phase 6: Final Reveal (25s)
- All walls return to normal scale
- Path lights restore to full intensity (200.0)
- "SPATIAL MAZE PROTOCOL COMPLETE" message
- True path revealed

## Playtest Checklist

### Pre-Test Setup
- [ ] Index3MovingWallsController placed and configured
- [ ] All wall groups properly tagged
- [ ] Path lights tagged and positioned
- [ ] System text actor tagged and visible
- [ ] Audio assets assigned (optional but recommended)

### Functionality Tests
- [ ] Controller starts sequence after initial delay
- [ ] Left walls move inward at 5s mark
- [ ] Corridor walls rotate at 8s mark
- [ ] Right walls move outward at 8s mark
- [ ] System messages appear at correct times
- [ ] Fake walls appear at 12s mark
- [ ] Path lights dim during disorientation
- [ ] Wall scaling works at 18s mark
- [ ] Final reveal restores normal state at 25s

### Player Experience Tests
- [ ] Player can navigate initial corridor
- [ ] Wall movements create claustrophobia
- [ ] Corridor changes redirect player path
- [ ] Fake walls create confusion
- [ ] Lighting changes affect visibility
- [ ] Final state provides clear exit path

### Debug Features
- [ ] Enable `bShowDebugMessages` for detailed console output
- [ ] Check on-screen debug messages during sequence
- [ ] Verify actor counts in debug output
- [ ] Confirm all tagged actors are found

## Troubleshooting

### Common Issues

**Walls not moving:**
- Verify actors have correct tags
- Check actor names contain expected strings
- Ensure walls are not static/immovable

**System text not appearing:**
- Confirm SYSTEM_TEXT tag on text actor
- Check text actor visibility and placement
- Verify TextRenderComponent exists

**Lights not dimming:**
- Ensure INDEX3_PATH_LIGHTS tag on light actors
- Check LightComponent exists on tagged actors
- Verify initial light intensity > 50

**Audio not playing:**
- Assign sound assets in controller properties
- Check sound asset paths are valid
- Verify audio settings in project

### Debug Commands
Enable debug messages in the controller to see:
- Actor caching results
- Sequence timing confirmations
- Wall manipulation feedback
- System message triggers

## Performance Notes

- Wall movements use SetActorLocation/Rotation (instant)
- No interpolation/animation by default
- Consider adding smooth movement for better visual effect
- Light intensity changes are immediate
- Sequence runs entirely on timers (no tick required)

## Extension Ideas

- Add smooth wall movement with interpolation
- Implement player proximity triggers
- Add particle effects for wall movements
- Create multiple sequence variations
- Add sound occlusion for moving walls
- Implement dynamic lighting color changes