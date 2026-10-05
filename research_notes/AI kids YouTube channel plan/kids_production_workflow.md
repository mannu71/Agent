# Production Workflow for an AI-Animated Educational Kids Channel (ages ~4–12), Open-Source / Wan2GP, No Local GPU — as of Oct 2026

## Q1. Consistent cartoon characters across episodes (methods, Wan2GP features, LoRA cost/time)

### Takeaway
The reliable 2026 recipe is: (1) design each character once and build a multi-angle/expression "character sheet" with an image-edit model (Qwen Image Edit / Qwen Image 2.1), (2) generate every shot's first frame from those fixed references (image-to-video), and (3) for recurring stars, train a cheap image LoRA (Flux.2 / Qwen-Image / Wan image LoRA: ~$1–15 on a rented 4090) rather than a full Wan 2.2 video LoRA (~$28–52 and 1–3 days per run by one estimate). Wan2GP (v13.141, 29 Sep 2026) now runs reference-driven models (MiniMax H3 Ref2VA with up to 3 reference videos + 3 audio refs, Viggle/Wan-Animate-style animation, in-context face/character-swap LoRAs) plus LoRA loading, but has no built-in LoRA trainer.

### Cited Findings
- Wan2GP ("WanGP: AI Generative Models for GPU-Limited Systems") supports video models Wan 2.1/2.2 and derivatives, MiniMax H3, LTX-2/2.3/2.5, Hunyuan Video 1/1.5, LongCat, Kandinsky, LTXV, MagiHuman; image models Krea 2, Qwen Image 2.1, Z-Image, Flux 1/2 (Klein, Chroma), HiDream and others; claims "as little as 6 GB of VRAM" for select models, GTX 10XX through RTX 50XX and AMD RDNA 2–4 — [Wan2GP README](https://github.com/deepbeepmeep/Wan2GP)
- Latest release v13.141 (29 Sep 2026): live video previews, "H3 Learns to Hold Still" (text-to-image single still), "three uploaded voice references now work with H3 Audio, Qwen3 TTS Base, OmniVoice". v13.1315 (24 Sep 2026) added Qwen Image 2.1 "with out-of-the-box editing capabilities"; v13.10 (16 Sep 2026) added YuE2 songs and a phone-friendly "Deepy Web App" — [Wan2GP README (raw)](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- Character-consistency features in current Wan2GP: "Ref2VA now accepts up to three reference videos and three audio references"; "H3 Ref2VA Frames Injection"; "H3 In-Context LoRAs: Use Control Video as In-Context Guide enables compatible face/character-swap LoRAs"; "Viggle Animate: a H3 Based model similar to Wan Animate but 3 steps only"; "H3 Ref2VA can now build a video around uploaded speech/music or a reference video's soundtrack" — [Wan2GP README (raw)](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- Wan2GP LoRA support: "adapt each model with LoRAs"; LoKr adapters for Krea 2, Qwen Image, Ming Image, Qwen Image 2.1; YuE2 accepts LoRAs. No built-in LoRA training documented — [Wan2GP README (raw)](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- Qwen Image Edit-based "Consistent Character Creator 3.0" ComfyUI workflow keeps facial structure, clothing and style coherent across angles; Qwen Image 2.1 workflows produce full character design sheets (turnarounds, expression studies, detail grids) from one reference image, which can be used as references for video models — [RunComfy](https://www.runcomfy.com/comfyui-workflows/consistent-character-creator-3-0); [PromptHero Qwen Image 2.1 sheet maker](https://prompthero.com/ai-models/qwen-image-21-character-design-sheet-maker-workflow-2960750-download/qwen-image-21-character-design-sheet-maker-workflow-qwen-image-21-v10); [FloYo: Qwen 2511 Edit single image → character dataset](https://www.floyo.ai/workflows/qwen-2511-edit-single-image-to-chara-65qytngb2sux)
- Wan 2.2 supports LoRAs that "teach the base model to recognize and consistently generate specific characters" — [MindStudio](https://www.mindstudio.ai/blog/what-is-wan-2-2-video-open-source)
- LoRA training costs (Spheron, updated 3 Oct 2026): Flux.2 LoRA on RTX 4090 = $1.16–2.32 per run (2–4 h), 15–30 images, 800–1,500 steps; budget 2–3 iterations = $5–15. Wan 2.2 *video* LoRA = $41–51 on A100 80G (24–30 h), $40–52 on H100 (20–26 h), $28–42 on RTX 4090 with block-swap (2–3 days), 20–40 clips, 3,000–5,000 steps. VRAM: Wan 2.2 image LoRA 12 GB+, video LoRA 24 GB+. Tools: ai-toolkit (MIT) for Flux.2, musubi-tuner for Wan 2.2. Spheron prices (3 Oct 2026): 4090 $0.48/h, A100 $1.48/h ($1.20 spot), H100 $2.98/h — [Spheron](https://www.spheron.network/blog/fine-tune-flux2-wan-lora-cost-gpu-cloud-2026/) (note: vendor blog promoting its own cloud)
- Conflicting training-time estimate: Wan 2.2 character/style LoRA 4–10 h on RTX 4090, 6–14 h on 3090, 3–6 h on A100 40GB for 200–400 samples; 768² takes 2.2× longer than 512² — [Apatero](https://apatero.com/blog/wan-22-video-lora-training-time-complete-analysis-2025). One real RunPod run cost ~$0.75 total (4090 at $0.69/h for 60 min + CPU pod) — [lilting.ch](https://lilting.ch/en/articles/runpod-lora-cpu-gpu-split-dollar)
- RunPod RTX 4090: $0.34/h Community Cloud, $0.69/h Secure Cloud; RTX 5090 ~$0.99/h on RunPod; Vast.ai 4090 $0.34–0.50/h — [Spheron RunPod vs Vast 2026](https://www.spheron.network/blog/gpu-cloud-pricing-comparison-runpod-vs-vastai-2026/); [Hackceleration RunPod pricing](https://www.hackceleration.com/labs/runpod-pricing)

### Inferences
- For stylized cartoon characters (simple shapes, flat colors), an image LoRA + strong first frames is usually enough; a Wan 2.2 video LoRA is mainly worth it for motion style/mannerisms. Budget ~$5–15 per character for image LoRAs; ~$30–50 per character if a video LoRA is needed (taking the more conservative Spheron figures; Apatero's suggest 4–10 GPU-hours ≈ $2–7 on community 4090s).
- Design characters to be "AI-friendly": distinctive silhouettes, few accessories, solid colors, no text on clothes, non-human/animal mascots (fewer uncanny-face issues, easier consistency). Numberblocks-style geometric characters are trivially consistent.
- Workflow: character sheet (Qwen Image 2.1 in Wan2GP) → shot keyframes with LoRA/image-edit → I2V (Wan 2.2 / LTX-2.x / H3 Ref2VA) → lipsync pass → edit. Keep a "bible" folder of canonical refs, seeds, prompts and LoRA versions.
- Prior knowledge (not re-verified in this session's fetches): Wan2GP has historically also shipped Wan VACE (reference/control-guided generation), Wan 2.2 Animate, Phantom, Lynx, Stand-In and HuMo identity/reference models; the README excerpt fetched only surfaced H3/Viggle features, so the writer should confirm current names in the repo's model dropdown/docs.

### Gaps
- No independent benchmark of character-identity retention for cartoon (vs photoreal human) characters across Wan 2.2 / LTX-2.x / H3 found.
- Whether LoRAs trained for Wan 2.2 work with MiniMax H3 / LTX-2.5 in Wan2GP was not confirmed (they almost certainly don't cross architectures; separate LoRAs per base model).
- No official Wan2GP RunPod template was found; community Docker/Pinokio installs exist but weren't verified.

## Q2. 2D/3D cartoon style quality in open models vs closed tools; lip-sync for cartoon characters

### Takeaway
Open models in Wan2GP (Wan 2.2, LTX-2.x, MiniMax H3) can produce usable 3D-Pixar-ish and flat-2D cartoon shots, especially from strong image first frames, and InfiniteTalk/MultiTalk-type audio-driven models give phoneme-level lip-sync with no hard length limit. I found no rigorous 2026 head-to-head of open vs closed tools for cartoon style specifically, so quality claims beyond "usable with curation" are unverified.

### Cited Findings
- InfiniteTalk: audio-driven lip-sync model built on Wan 2.1 I2V; single- and multi-character variants for dialogue; "streaming pipeline that processes audio in overlapping segments with no hard length limit" (up to ~10 min per generation); drives the full face (eyebrows, smiles, head tilts) — [lilting.ch](https://lilting.ch/en/articles/infinitetalk-comfyui-lipsync); [ComfyUI Wiki](https://comfyui-wiki.com/en/models/wan/infinitetalk); official ComfyUI templates include an InfiniteTalk *music* (singing) workflow — [comfy.org](https://comfy.org/workflows/templates-wan2_1_infinitetalk_music-1eab7aa23f6a/). Note: one source attributes InfiniteTalk to ByteDance/InfiniteYou; to my knowledge it is from MeiGen-AI (the MultiTalk team) — treat attribution as unconfirmed.
- Wan2GP lists "lip-sync through soundtrack synchronization" and multi-speaker support (up to three voices per generation), and H3 Ref2VA can build a video around uploaded speech/music — [Wan2GP README](https://github.com/deepbeepmeep/Wan2GP); [raw README](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- LTX-2/2.3/2.5 and MiniMax H3 are both supported in Wan2GP — [Wan2GP README](https://github.com/deepbeepmeep/Wan2GP)

### Inferences
- For kids content, lip-sync matters most for narrator/host characters; songs can use InfiniteTalk's music mode or simply avoid close-up mouths (wide shots, dancing, objects) to cut cost.
- Cartoon mouths (simple shapes) are generally more forgiving than photoreal; audio-driven models trained mostly on humans may over-animate faces on non-human mascots — test with the actual character before committing.
- Practical style choice: soft 3D "toy/claymation" or flat 2D with thick outlines; avoid styles that mimic specific studios (Pixar/Disney/Cocomelon) both for IP risk and YouTube's "strange use of children's characters" rule.

### Gaps
- No sourced quality comparison of Wan 2.2 vs LTX-2.x vs H3 vs closed tools (Veo, Kling, Runway, Sora) on cartoon content; no sourced generation-time per clip on 4090/5090 for these models in Wan2GP.
- No sourced evaluation of InfiniteTalk on non-human cartoon characters.

## Q3. Kid-friendly AI voices (English and Hindi): licenses and quality

### Takeaway
Fully commercial-safe open options exist: Kokoro (Apache-2.0, has 2 Hindi voices, very fast), Chatterbox / Chatterbox Multilingual (MIT, 23 languages incl. Hindi, zero-shot cloning, "exaggeration" control useful for cartoon voices), Indic Parler-TTS (Apache-2.0, 22 Indian languages); Wan2GP itself bundles Qwen3 TTS, Index TTS2/2.5, OmniVoice, Chatterbox, KugelAudio and more. For best Hindi quality at low cost, Sarvam Bulbul v3 API (~₹30 per 10k characters) and ElevenLabs (commercial from ~$5–6/mo Starter) are the paid fallbacks.

### Cited Findings
- Wan2GP audio models: Qwen3 TTS, MiniMax H3 Voice Clone, Ace Step 1/2/XL, OmniVoice, Index TTS2/2.5, KugelAudio, HeartMula, Chatterbox, MiniMax Music, Stable Audio 3, YuE2, AuK Speech (instruction-driven speech or voice cloning); voice cloning and speaker diarization; three voice references in one conversation (v13.141). No language/Hindi info given — [Wan2GP raw README](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- Kokoro (Hexgrad): 82M params, StyleTTS + ISTFTNet, ~1,200 h training data, ~100× real-time on GPU; languages include Hindi with voices "Alpha" (female) and "Omega" (male); Apache-2.0, commercial use allowed — [tts.ai Kokoro](https://tts.ai/voices/kokoro/)
- Chatterbox / Chatterbox Multilingual (Resemble AI): MIT license, 23 languages including Hindi, zero-shot voice cloning, 0.5B Llama backbone, ~0.5M h training data, "exaggeration/intensity control"; Resemble claims it is preferred over ElevenLabs in side-by-side evals (vendor claim) — [Chatterbox README on HF](https://huggingface.co/ResembleAI/chatterbox/blob/refs%2Fpr%2F48/README.md); [Resemble AI](https://www.resemble.ai/products/text-to-speech)
- Indic Parler-TTS (AI4Bharat): Apache-2.0, 22 Indian languages incl. Hindi; one aggregator says to contact AI4Bharat for commercial terms (conflicts with Apache-2.0 which already permits commercial use) — [dev.co](https://dev.co/ai/llms/indic-parler-tts); [tts.ai](https://tts.ai/voices/indic-parler/)
- Sarvam Bulbul v3: 11 Indian languages, 35+ voices, emotion control; free 1,000 credits, then ₹30 per 10K characters (beta, Aug 2026) — [Sarvam TTS](https://www.sarvam.ai/tts-v2); [InVideo blog](https://invideo.io/blog/sarvam-bulbul-indian-tts/)
- ElevenLabs 2026: Free/Starter/Creator/Pro/Scale/Business; Starter ~$5/mo with 30k credits (~30 min TTS) and commercial license; Creator $22/mo, 100k credits (~100 min), Professional Voice Cloning; supports Hindi narration — [Smallest.ai](https://smallest.ai/blog/elevenlabs-pricing-plans-cost-what-you-get-in-2026); [Zoutons India pricing May 2026](https://www.zoutons.com/news/elevenlabs-pricing-india-may-2026). Conflict: another source says commercial use requires the "$6 Starter plan" — same search results.

### Inferences
- Suggested stack: English narrator = Kokoro or Chatterbox (pitch/"exaggeration" for character voices); Hindi = Chatterbox Multilingual or Indic Parler-TTS, with Sarvam Bulbul as quality fallback (a 5-min script ≈ 4–5k characters ≈ ₹15/episode).
- For cartoon character voices, clone from consented/own recordings (e.g., creator performing a voice, then pitch-shift) — avoid cloning celebrities or existing cartoon voices. Children's real voices should not be cloned without parental consent.
- TTS for young kids should be slower (~110–130 wpm) with clear pauses; inference, not sourced.

### Gaps
- No independent MOS/quality comparison found for Hindi child-like/cartoon voices across Kokoro, Chatterbox, Indic Parler, Bulbul.
- Qwen3 TTS / Index TTS2 / OmniVoice licenses and Hindi support not verified this session.

## Q4. AI music/songs for kids: open models, licenses, Suno/Udio terms, Content ID risk

### Takeaway
Open, commercially usable song generators are now good enough for nursery-style songs and run inside Wan2GP: ACE-Step 1.5 (MIT/Apache, trained on licensed/royalty-free/synthetic data) and YuE/YuE2 (Apache-2.0, lyrics→full song with vocals). Avoid Stable Audio *Open* (non-commercial); Stable Audio 3 is commercial under a <$1M-revenue community license. Suno requires a paid plan for commercial rights (now framed as a license post-Warner deal, with download caps), and any AI song can still trigger Content ID false matches.

### Cited Findings
- ACE-Step 1.5: MIT or Apache-2.0 (sources differ), permits commercial use of outputs; trained on licensed tracks, royalty-free data and synthetic MIDI-rendered audio; no indemnification — [dev.co](https://dev.co/ai/llms/acestep-5hz-lm-4b); [HF model card](https://huggingface.co/ACE-Step/acestep-v15-base/blob/main/README.md); [ComfyUI Wiki](https://comfyui-wiki.com/en/models/ace-step/ace-step-v1-5); AMD markets it as "commercial-grade" — [AMD blog 2026](https://www.amd.com/en/blogs/2026/commercial-grade-ai-music-generation-on-amd-ryzen-ai-and-radeon-ace-step-1-5.html)
- YuE relicensed to Apache-2.0 (30 Jan 2025) — [HF commit](https://huggingface.co/m-a-p/YuE-s1-7B-anneal-en-cot/commit/454c20e1748888800f8e4b3da45125f55482d967). Wan2GP added YuE2 ("turn lyrics and a musical style into a complete song with vocals and accompaniment") in v13.10, 16 Sep 2026, and vocal removal for instrumentals — [Wan2GP raw README](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/README.md)
- Stable Audio Open: non-commercial research license; Stable Audio 3.0 (May 2026) commercial under Community License for < $1M annual revenue, Enterprise above — [Conductatlas](https://conductatlas.com/platform/stability-ai/stability-ai-model-license/); [Digital Music News](https://www.digitalmusicnews.com/2026/05/21/stability-ai-3-0-release/); [Stability AI](https://stability.ai/explainers/stable-audio-vs-competitors-licensing-export-rights-and-self-hosting-compared)
- Suno after Warner partnership (download restrictions announced 25 Nov 2025): free-tier songs not commercially usable and not downloadable; paid users get commercial rights but face monthly download caps; terms shifted from "ownership" toward granted commercial license — [Music in Africa](https://www.musicinafrica.net/magazine/suno-adjusts-ai-music-ownership-terms-after-warner-music-partnership); [Dubspot](https://blog.dubspot.com/ai-music-licensing-explained-2026)
- Content ID: even original AI music can be flagged if a melody resembles a copyrighted song; generator terms, distributor policy and Content ID are separate systems ("Suno says I can monetize it" ≠ Content ID accepts it); DistroKid has started flagging AI content — [Jack Righteous](https://jackrighteous.com/blogs/guides-using-suno-ai-music-creation/distribute-suno-music-after-september-3-spotify-distrokid-youtube); [licenseorg](https://licenseorg.com/blog/ai-music-licensing-suno-elevenlabs)

### Inferences
- Use original lyrics (not public-domain nursery rhyme *recordings*; public-domain lyrics like "Twinkle Twinkle" are fine but melodies may match many Content ID-registered covers → high false-claim risk). Original melodies from ACE-Step/YuE2 with simple 3–4 chord structures are lower risk.
- Do not register your own AI songs in Content ID via distributors unless sure they are distinct — they could claim others or trigger disputes.
- Udio's 2026 terms not researched (gap).

### Gaps
- Udio 2026 commercial/download terms; Suno's current plan prices; whether Suno's Warner-licensed models changed Content ID behavior.
- No data on how often ACE-Step/YuE outputs get Content ID claims.
- Hindi lyric quality of ACE-Step/YuE2 not verified.

## Q5. Content design that retains young viewers and meets "high-quality" principles

### Takeaway
Retention and quality now pull in the same direction for AI channels: YouTube rewards (and YouTube Kids admits) content that is enriching, has a clear storyline, and avoids "mass-produced/autogenerated," keyword-stuffed or sensational videos, and its July 2026 clarification explicitly demonetizes generic/template AI content. Research favors slower pacing for preschoolers (Lillard: fast-paced cartoons halved 4-year-olds' executive-function performance), short focused segments (Numberblocks 2–5 min), recurring characters, songs and repetition, and a concrete learning objective per episode.

### Cited Findings
- YouTube high-quality principles for kids content (developed with child-development specialists): looking after yourself and others; learning and inspiring curiosity; creativity, play and imagination; life skills with positive role models; showing them the world (diversity). Low-quality: heavily promotional, encouraging negative behaviors, "deceptively educational," "hard to follow" (jumbled storylines, unclear audio — "often the result of mass production or autogeneration"), sensational/misleading incl. keyword stuffing, "strange use of children's characters." High-quality content is raised in recommendations; low-quality focus can lead to YPP suspension or limited ads — [YouTube Help 10774223](https://support.google.com/youtube/answer/10774223?hl=en)
- July 16, 2026 YouTube monetization clarification: three non-monetizable "inauthentic content" categories — (1) generic, repetitive or template-based content ("cookie-cutter videos" made with AI, CGI or templates), (2) off-putting/distressing content (e.g., animal-in-distress-then-rescued videos), (3) AI personas discussing sensitive topics (health, finance, legal). YouTube's Matt Halprin: the tech "enables great stuff, but it also enables … content farming" — [TechCrunch, 20 Jul 2026](https://techcrunch.com/2026/07/20/youtube-clarifies-policies-around-ai-slop-and-upsetting-videos/). Policy was renamed from "repetitious" to "inauthentic content" on 15 Jul 2025 — [ScaleLab](https://scalelab.com/en/why-youtube-is-cracking-down-on-ai-generated-content-in-2026)
- April 2026: Fairplay-led open letter from ~200 organizations/experts (135 orgs incl. American Federation of Teachers; Jonathan Haidt) asked YouTube/Google to stop recommending "AI slop" to kids; cites reports that 40% of recommended videos following popular preschool shows like Cocomelon contained AI content and 21% of new users' recommended Shorts contained AI slop — [Tubefilter](https://www.tubefilter.com/2026/04/01/youtube-fairplay-kids-ai-open-letter/); [Fairplay](https://fairplayforkids.org/youtube-stop-ai-slop-for-kids-says-letter-from-fairplay-over-200-experts-including-jonathan-haidt/); [Fortune](https://www.fortune.com/2026/04/01/ai-slop-200-organizations-letter-youtube-google)
- Secondary/unverified: one creator-economy blog claims enforcement for Made-for-Kids AI content tightened after the letter, making MFK AI monetization "practically very difficult," and advises building revenue on brand deals/licensing — [ytgrowth.io](https://ytgrowth.io/blog/youtube-ai-policy) (no primary YouTube statement found confirming a kids-specific change)
- Lillard & Peterson (UVA, 2011): 60 four-year-olds; 9 min of a fast-paced cartoon (SpongeBob, scene change every ~11 s) vs slower educational show (Caillou, ~34 s) vs drawing; fast-paced group performed ~half as well on executive-function tasks immediately after — [HealthDay](https://www.healthday.com/healthpro-news/child-health/fast-paced-tv-impairs-executive-function-in-prechoolers-656735.html); [UVA PDF](https://uva.theopenscholar.com/files/early-development-lab/files/the_immediate_impact_8.pdf)
- Moonbug/Cocomelon: child-development consultants shape music, structure, pacing, repetition and everyday subject matter; tests episodes with kids using a "Distractatron" (a second screen of mundane scenes; researchers log look-aways); scene transitions reportedly every 1–2 s; April 2026 formal partnership with UCLA Center for Scholars & Storytellers on a framework (real-life moments, positive relationships, learning through play, inclusive stories) — [UGA-hosted article](https://agtechdata.uga.edu/cocomelon-aims-to-debunk-addicting-concerns-with-learning-principles/); [LA Mag](https://lamag.com/contributor-content/cocomelon-and-uclas-center-for-scholars-storytellers/); [NetInfluencer](https://www.netinfluencer.com/moonbug-taps-ucla-research-partnership-to-counter-cocomelon-overstimulation-claims/)
- Numberblocks: numbersongs 2–3 min, main episodes ~5 min, specials 10–20 min; characters literally made of unit blocks, so the character design *is* the math concept — [Wikipedia](https://en.wikipedia.org/wiki/Numberblocks); [Common Sense Media](https://www.commonsensemedia.org/tv-reviews/numberblocks)
- Blippi: created by a former preschool teacher; consistent costume/persona, direct address, catchy songs, real-world exploration of colors/shapes/letters/numbers — [Heads Up For Tails](https://connect.headsupfortails.com/agathe/meet-blippi-everything-you-need-to-know.html)

### Inferences
- Age split: 3–6 → 2–5 min songs/stories, slower cuts (aim nearer the 30 s/scene of educational shows than 1–2 s Cocomelon cuts), one concept per episode, call-and-response pauses, repetition of the target word/number 5–10×, recurring host + 2–3 friends; compile into 20–60 min compilations (common kids-channel practice, unsourced). 7–12 → 6–12 min Kurzgesagt-lite explainers with a question hook, 3-act structure, recap and quiz.
- To avoid "template/inauthentic" demonetization: a named recurring cast with personalities and story arcs, a real written script per episode (ideally reviewed by a teacher), varied settings/structures, original songs, human voice direction, and visible editorial value. Avoid mass-uploading near-identical videos and "animal in distress" hooks.
- Disclosure: tick YouTube's altered/synthetic content disclosure where applicable; Made-for-Kids setting disables comments and personalized ads (standard COPPA settings; not re-verified here), lowering RPM — weigh English vs Hindi CPM and possibly a "kids & family" (not MFK) 7–12 channel.

### Gaps
- No primary YouTube source on 2026 Made-for-Kids-specific AI enforcement changes.
- No published retention benchmarks (average view duration %) for AI kids channels.
- Sesame Workshop / Common Sense Media 2026 research on AI kids content not fetched.

## Q6. Realistic weekly time and cost budget (no local GPU)

### Takeaway
A solo creator renting a community-cloud RTX 4090/5090 ($0.34–0.99/h) can plausibly produce one ~5-minute polished episode plus a song/short per week for roughly $15–40/week in compute, plus a one-off ~$20–100 for character LoRAs, with ~15–25 hours/week of human time (scripting, curating retakes, editing being the bottleneck). These numbers are inferences from sourced GPU prices; I found no sourced end-to-end per-episode cost study.

### Cited Findings
- RunPod 4090 $0.34/h (community) / $0.69/h (secure); RunPod 5090 ~$0.99/h; Vast.ai 4090 $0.34–0.50/h — [Spheron comparison](https://www.spheron.network/blog/gpu-cloud-pricing-comparison-runpod-vs-vastai-2026/); [Hackceleration](https://www.hackceleration.com/labs/runpod-pricing)
- LoRA costs: Flux.2 image LoRA $5–15 incl. iterations; Wan 2.2 video LoRA $28–52/run — [Spheron](https://www.spheron.network/blog/fine-tune-flux2-wan-lora-cost-gpu-cloud-2026/)
- Paid voice fallbacks: Sarvam Bulbul ₹30/10k chars — [Sarvam](https://www.sarvam.ai/tts-v2); ElevenLabs Starter ~$5/mo, Creator $22/mo — [Smallest.ai](https://smallest.ai/blog/elevenlabs-pricing-plans-cost-what-you-get-in-2026)
- Wan2GP runs select models in 6 GB VRAM and Qwen/Deepy prompt enhancer in ~10 GB, so mid-range cloud GPUs suffice — [Wan2GP README](https://github.com/deepbeepmeep/Wan2GP)

### Inferences
- Rough compute math (assumptions, not sourced): 5-min episode ≈ 50–70 shots of ~5 s; with 2–3 takes each ≈ 150–200 generations; at ~3–8 min/generation on a 4090/5090 ≈ 10–25 GPU-hours ≈ $4–25; add keyframe images, TTS, music, lip-sync and idle/setup time → ~$15–40/week. Persistent storage for models (Wan 2.2 14B + LTX + audio models easily 100+ GB) adds a monthly volume fee (rate not sourced).
- Time: script + lesson design 3–4 h; character/keyframes 3–5 h; video generation supervision 4–8 h (batch queue overnight); voice + music 2–3 h; edit/captions/thumbnail (CapCut/DaVinci Resolve free) 4–6 h → ~15–25 h/week for one long episode + 2–3 Shorts.
- Cost-saving: stop pods when idle, use spot/interruptible instances for queued batches, pre-build a Docker image with models on a network volume, render drafts at low res/steps then upscale finals.
- Free/cheap alternatives without GPU: Kokoro and Chatterbox run on CPU (slowly) or free Colab/Kaggle tiers (not verified for 2026 quotas).

### Gaps
- No sourced per-clip generation time for Wan 2.2 / LTX-2.5 / H3 at 720p on 4090/5090 in Wan2GP.
- RunPod network-volume storage pricing and per-second billing details for 2026 not verified.
- No case study of an actual AI kids channel's weekly costs/revenue found.
