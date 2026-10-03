# What If Itihaas — Channel Style Guide

**Handle:** @WhatIfItihaas
**Tagline:** *The epics, one different choice at a time.*
**Language:** English first (Hindi audio track via YouTube auto-dubbing; a Hinglish channel only if the dub gets real watch time)

---

## 1. What the channel is

Respectful, well-sourced "what if" episodes on the Mahabharata (later the Ramayana and Indian history). Each episode:

1. Tells what **actually happens** in the epic, with sources.
2. Picks **one fork**: a single decision that could have gone differently.
3. Reasons through the alternate timeline, **clearly labelled as speculation**.
4. Ends on the moral question the epic itself raises, and asks viewers to vote.

**Standing disclaimer (pinned comment and description):**
> What If Itihaas explores the Mahabharata with respect. "What if" scenarios are thought experiments, not claims about scripture or faith. Sources are listed below.

## 2. Voice and narrator

| Item | Choice |
|---|---|
| Narrator persona | Calm storyteller, never a "guru" or expert persona (YouTube won't monetize AI posing as an expert) |
| Voice | Deep, warm, unhurried male voice. Test Kokoro (free) first; switch to ElevenLabs Starter ($5–6/mo) if it sounds flat |
| Pace | ~140–150 words/minute; pause 1 s after each section |
| Pronunciation | Keep a pronunciation sheet (Kuntī, Yudhiṣṭhira, Kurukṣetra). Spell names phonetically in the TTS input if the voice gets them wrong |
| Tone rules | No mocking deities or characters; present multiple interpretations; say "the text says" vs "we imagine" |

## 3. Visual style

**Look:** painterly cinematic illustration: oil-painting texture, dramatic light, dust and firelight. Not photorealistic. This keeps the look consistent, suits mythology, and is lower-risk for religious sensitivity than photoreal AI faces of deities.

**Prompt suffix (paste after every image prompt):**
```
painterly cinematic illustration, ancient India epic, oil painting texture,
dramatic chiaroscuro lighting, saffron and deep indigo palette, volumetric dust,
highly detailed, 16:9 composition
```

**Negative prompt:**
```
photorealistic, modern clothing, text, watermark, logo, cartoon, anime, extra limbs, deformed hands
```

**Mix per video:**
- ~75% AI still images with slow pan/zoom (Ken Burns) in DaVinci Resolve
- ~15% Wan2GP image-to-video clips (5–10 s) for key moments
- ~10% maps, family-tree diagrams, on-screen quotes

### Colour palette

| Name | Hex | Use |
|---|---|---|
| Deep Indigo | `#1B1F3B` | Backgrounds, night scenes, lower thirds |
| Saffron | `#E8891C` | Highlights, "WHAT IF" label |
| Epic Gold | `#D4AF37` | Titles, divine elements |
| Bone | `#F2E8D5` | Body text, quote cards |
| Blood Red | `#8B1E1E` | War, death, the "fork" moment |

### Fonts (all free, Google Fonts)

| Use | Font |
|---|---|
| Episode titles, quote cards | **Cinzel** (Bold) |
| Thumbnail text | **Bebas Neue** |
| Devanagari (Hindi titles, Sanskrit terms) | **Tiro Devanagari Hindi** |

## 4. Character reference sheets (make once, reuse forever)

Generate each character once with Z-Image or Qwen-Image in Wan2GP (front and three-quarter views, neutral background). Save the best image as that character's **reference**. Every later scene starts image-to-video from a reference, so characters stay consistent.

| Character | Fixed look |
|---|---|
| **Karna** | Tall warrior in his 30s, dark long hair, golden armour fused to his chest and glowing golden earrings (before Indra), sun motif on shoulder guard; after giving the armour: bare scarred chest, plain bronze armour |
| **Arjuna** | Lean archer, dark hair tied up, white and silver armour, the Gandiva bow |
| **Krishna** | Traditional iconography: blue skin, peacock feather in crown, yellow silk (pitambara), serene expression. **Always dignified, never comic** |
| **Kunti** | Royal woman in her 50s, white widow's sari, grey-streaked hair |
| **Duryodhana** | Broad-shouldered, black-and-gold armour, heavy mace |
| **Bhishma** | Tall elder, long white beard, white robes, silver armour |

## 5. Thumbnail template

- **Left two-thirds:** one character's face, close-up, strong emotion, lit from one side
- **Right third:** 2–4 words in Bebas Neue, Bone text with black outline, one word in Saffron
- **Top-left corner:** small Saffron tag reading **WHAT IF?**
- Examples: `KARNA SAYS YES?` · `NO WAR AT ALL?`
- Make two versions per video and use YouTube's thumbnail A/B test

## 6. Episode structure (vary it so episodes don't look templated)

| Default structure | Time |
|---|---|
| Hook: the most dramatic moment, then the question | 0:00–0:40 |
| The real story | ~35–40% |
| The fork: the exact moment we change | ~1 min |
| The alternate timeline (3–5 consequences) | ~35–40% |
| Verdict + the epic's moral question + viewer vote | last 1–2 min |

Rotate with other formats every few episodes: **countdown** ("5 moments that could have stopped the war"), **two-sided debate** ("Was Karna right?"), **chain reaction** (one change and its ripple effect through all 18 days).

## 7. Sources (cite in every description)

- *The Mahabharata*, Critical Edition, Bhandarkar Oriental Research Institute (BORI), Pune
- K. M. Ganguli, *The Mahabharata of Krishna-Dwaipayana Vyasa* (1883–1896, public-domain English translation)
- Bibek Debroy, *The Mahabharata* (Penguin, unabridged translation of the Critical Edition)
- When versions disagree (regional retellings, TV serials, the vulgate vs Critical Edition), say so on screen.

## 8. Upload checklist

- [ ] Facts checked against at least one translation above
- [ ] Speculation clearly labelled in narration and on screen
- [ ] "AI use" ticked in YouTube Studio if any scene looks realistic (painterly style is usually exempt, but tick it when unsure)
- [ ] Audience: **No, not made for kids**
- [ ] Description: disclaimer, sources, timestamps, music credit
- [ ] Pinned comment asking the episode's vote question
- [ ] 2–4 Shorts cut, each ending "Full story on What If Itihaas"
- [ ] Added to the "Mahabharata What Ifs" playlist
