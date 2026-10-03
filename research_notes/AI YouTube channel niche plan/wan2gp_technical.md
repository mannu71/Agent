# Wan2GP (WanGP) — Technical Evaluation for a No-GPU YouTube Creator (as of 3 Oct 2026)

Primary sources read directly: the repo README (raw), docs/CHANGELOG.md, docs/MODELS.md, docs/GETTING_STARTED.md, docs/PROCESSING.md, docs/TROUBLESHOOTING.md and docs/CLI.md, all on the `main` branch, fetched 3 Oct 2026. Repo: https://github.com/deepbeepmeep/Wan2GP. Throughout these notes, "README" means https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md and "CHANGELOG" means https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md.

Naming: the app now brands itself **"WanGP"**. The repo is still called Wan2GP. Despite the name, it is no longer a Wan-only app. It is a multi-model "super app" for video, image, audio and TTS.

---

## 1. What is Wan2GP, who maintains it, and how active is it?

### Takeaway
WanGP is a free, open-source Gradio web app by the pseudonymous developer **DeepBeepMeep**. It runs many open-weight video, image and audio models with heavy VRAM and RAM optimization ("for the GPU Poor"). It is **extremely active**: major versions ship about every 1–2 weeks through 2026, and the latest is **v13.141 on 29 Sep 2026**. Community code contributions are now merged too.

### Cited Findings
- Tagline: "WanGP by DeepBeepMeep: The best Open Source Generative Models Accessible to the GPU Poor". It describes itself as "a one-stop super app for the best open source generative models across video, image, audio, and text-to-speech." — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- "WanGP is free to use locally… will never ask you to pay a license fee, subscription, or donation to run WanGP on your own computer." It warns that only the GitHub repo and the wangp.ai / wan2gp.ai sites are official, and that the project is "not affiliated to any other third-party service using the WanGP/Wan2GP names." — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Official site https://wangp.ai/, Discord https://discord.gg/g7efUW9jGV, news on X @deepbeepmeep — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Latest entries:
  - **29 Sep 2026, v13.141 "Community Release"**: live video previews, H3 Ref2VA with up to 3 reference videos and audio, and H3 in-context LoRAs. Contributions are credited to community GitHub users such as @Beanow, @GOvEy1nw and @kito408.
  - **24 Sep 2026, v13.1315**: Qwen Image 2.1, just-in-time checkpoint downloads, and Comfy Kitchen kernels that make H3 and LTX2.x about 10% faster.
  - **16 Sep 2026, v13.10**: new UI, "Access Anywhere" remote browser access, workspaces, YuE2 music, LTX 2.5 updates.
  - **6 Sep 2026, v12.72**: DLSS 5 neural rendering and upscaling, DLSS frame generation up to x6.
  - Source: [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Release cadence in the 2026 changelog: Jan 1 (v10.01), Jan 9 (v10.11, LTX-2 added), Jan 13, Jan 15, Jan 20, Jan 29; Feb 1, 4, 12, 16, 19; Mar 7, 17, 30; Apr 8, 11, 21, 25; May 2, 9, 12, 21, 29; Jun 1, 4, 7, 14, 26; Jul 1, 29; Aug 5 (v12.42, MiniMax H3 added), Aug 6, 9, 16, 19, 26. That is roughly **35+ dated releases in 9 months**. — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- The project dates back to at least v5.4 on 6 Jun 2025 in the changelog. Its earlier focus was Wan 2.1 and VACE. — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- The developer also maintains sibling "GP" (GPU-poor) projects: HunyuanVideoGP, Hunyuan3D-2GP, FluxFillGP, Cosmos1GP, OminiControlGP and YuEGP. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Third-party Wan2GP desktop launcher: "Wan2GP Desktop" (Tauri) by community member GKArtist. — [README](https://github.com/GKartist75/Wan2GP-Desktop-Tauri)

### Inferences
- The project has a single lead maintainer, so there is some bus-factor risk. On the other hand, the release velocity is exceptional, and new open models usually get day-one or near-day-one support with low-VRAM optimizations.
- The speed of change cuts both ways. Docs lag the code: GETTING_STARTED.md still recommends Python 3.10.9 and "Wan 2.1 1.3B" as the first model. The UI and settings also change often. Pin a known-good version if you need a reproducible production pipeline.

### Gaps
- I could not get GitHub star, fork or open-issue counts or GitHub Releases data: API access to the repo was blocked in this session. It is unclear whether formal GitHub Releases or tags are used. The changelog lives in README and docs/CHANGELOG.md.
- I did not review the Discord or Reddit sentiment directly.

---

## 2. Which models and features does it support right now?

### Takeaway
As of v13.141, WanGP supports most major open-weight video models:
- Wan 2.1/2.2 and many derivatives (VACE, Animate, InfiniteTalk, MultiTalk, Bernini, SCAIL-2 and others)
- **MiniMax H3**, the current top open-weight video model
- LTX-2 / 2.3 / 2.5 with native audio
- HunyuanVideo 1 / 1.5, LongCat, Kandinsky 5, Ovi and MagiHuman

It also bundles strong image models (Krea 2, Qwen Image 2.1, Flux 1/2, Z-Image, Ideogram 4) and TTS/music models (Qwen3 TTS, IndexTTS2, ACE-Step, YuE2, Stable Audio 3). **Wan 2.5, 2.6, 2.7 and 3.0 are NOT supported** because Alibaba never released their weights.

### Cited Findings: model list
- **Video:** "Wan 2.1/2.2 and derived models, MiniMax H3, LTX-2/2.3/2.5, Hunyuan Video 1/1.5, LongCat, Kandinsky, LTXV, MagiHuman" — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Image:** "Krea 2, Qwen Image, Z-Image, Flux 1/2 (Klein, Chroma), SenseNova, Ideogram 4, HiDream" — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Audio / TTS:** "Qwen3 TTS, MiniMax H3 Voice Clone, Ace Step 1/2/XL, Omnivoice, Index TTS2/2.5, KugelAudio, HeartMula, Chatterbox, Minimax Music, Stable Audio 3." YuE2 songs and AuK Speech were added in v13.10. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)

### Cited Findings: officially recommended starting points
All from [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md):
- **Cinematic video with native audio:** LTX-2.3 Distilled 1.1. It supports start/end frames, control video, references, outpainting, injected frames and sliding windows.
- **Connected stories with recurring characters:** JoyAI-Echo Surgical, which "uses reusable memories across shots to retain characters, voices, objects, and locations."
- **General T2V / I2V:** Wan 2.2 T2V / I2V, described as "mature general-purpose video models with broad LoRA and WanGP feature support." Wan 2.2 TI2V 5B is the smaller path.
- **Editing / outpainting:** Wan VACE, Bernini-R.
- **Character animation:** SCAIL-2, Wan 2.2 Animate.
- **Talking heads:** LongCat Avatar 1.5, InfiniteTalk, MultiTalk.
- **Other video:** Vista4D (re-shoot a scene from a new camera trajectory), Lynx (identity-preserving face replacement), Kandinsky 5 Pro (19B, "controllable camera motion"), HunyuanVideo 1.5 (8.3B), Ovi (video plus a synchronized soundtrack), Magi Human (audio-driven talking heads).
- **Legacy, still available:** HunyuanVideo, LTX-Video 13B, Phantom, Recam, SkyReels Diffusion Forcing, Wan-Fun and others.

### Cited Findings: MiniMax H3
- Added 5 Aug 2026 in v12.42. The changelog calls it "a top-notch open-weight contender to Seedance 2, combining cinematic video generation, convincing motion, strong prompt adherence, and a synchronized native stereo soundtrack in one model." — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- Variants:
  - FL2VA: text / first-frame / last-frame to video plus audio, with sliding windows.
  - Ref2VA: reference images, videos and audio.
  - Each comes as a full 33B model and a pruned 20B model.
  - LoRA accelerators are available, and "Viggle Animate" (3-step H3 animation) was added in v12.72.
  - Sources: [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md), [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)

### Cited Findings: features
- **Generation modes:** text-to-video, image-to-video, first/last frame, video continuation, sliding windows (long video), multi-prompt per window, and hard cuts via `[/new_shot]`. — [docs/PROCESSING.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/PROCESSING.md)
- **LoRAs and model formats:** LoRA support for each model, reuse of LoRAs from other apps, custom finetunes from HF or CivitAI, a CivitAI browser plugin, and quantized formats (int8, fp8, GGUF, NV FP4, Nunchaku). — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Upscaling and post-processing:** RIFE, FlashVSR, Lanczos, SeedVR2, DLSS 5 neural refine and upscale, DLSS frame generation up to x6 (RTX 40/50 only), MMAudio soundtrack generation, SeedVC voice replacement, and vocal removal. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md); [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Control:** pose, depth, edge, flow, inpainting, outpainting, a mask editor and a background remover. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Automation:** per-model prompt enhancer, a generation queue, headless batch mode (`python wgp.py --process my_queue.zip`) and the WanGP API. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **"Deepy" offline agent:** an LLM assistant that orchestrates jobs. Deepy Prime needs about 10–16 GB VRAM depending on configuration. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md); [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Camera control:** Kandinsky 5 Pro's controllable camera motion, Vista4D camera re-trajectory, and the Recam legacy model. — [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md)
- **Lip-sync:** InfiniteTalk, MultiTalk, LongCat Avatar 1.5, Magi Human, LTX-2 "force your soundtrack," and H3 Ref2VA "Soundtrack Kept." — [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md); [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)

### Cited Findings: Wan open-weights status
- The open Wan line stops at **Wan 2.2**:
  - "the official Wan-AI organisation on Hugging Face publishes weights only up to Wan 2.2." Wan 2.5, 2.6 and 2.7 were API-only. — [howaiworks.ai](https://howaiworks.ai/blog/alibaba-wan-open-weights-stopped-at-2-2)
  - Wan 3.0, released 24 Aug 2026, offers up to 30 s at 1080p for **$0.20 per output second** through Alibaba's API, with "no downloadable weights." — [TheNextWeb](https://thenextweb.com/news/alibaba-wan3-video-model-after-share-sale); [Evolink](https://evolink.ai/blog/wan-3-0-release)
- **Conflict to flag:** [runaihome.com](https://runaihome.com/blog/wan-video-local-ai-gpu-guide-2026/) claims Wan 2.7 is "Available under Apache 2.0 on Hugging Face." This is contradicted by [howaiworks.ai](https://howaiworks.ai/blog/alibaba-wan-open-weights-stopped-at-2-2) and by the WanGP model list, which has no Wan 2.5+ entries. Treat the runaihome claim as wrong.

### Inferences
- For YouTube b-roll, the most relevant models are:
  - **MiniMax H3** for top quality, with audio.
  - **LTX-2.3/2.5 Distilled** for speed, long clips and native audio.
  - **Wan 2.2 I2V plus Lightning LoRAs** for mature tooling and LoRA ecosystem. Pair it with Krea 2 / Qwen Image / Flux for first frames.
- For Shorts with talking characters: InfiniteTalk, LongCat Avatar or H3 Ref2VA, with TTS from Qwen3 TTS or IndexTTS2. That whole pipeline lives inside one app.

### Gaps
- There is no single official table of exact max resolution per model; it varies by model and settings.
- Licensing is only noted here, not analysed. Each model has its own license. Wan 2.1/2.2 are Apache 2.0; others, such as LTX and MiniMax, have their own terms. Another researcher covers this.

---

## 3. VRAM/RAM requirements, generation times, length/resolution limits, and quality vs. closed tools

### Takeaway
WanGP's optimizations let headline models run on very little VRAM (6–12 GB), but **system RAM is the hidden requirement**: 32 GB is the practical minimum, and the developer often references 64 GB. Speed scales with GPU class. On a 24–32 GB card (RTX 4090/5090), the following are realistic:
- Wan 2.2 with 4-step Lightning LoRAs: **about 1–3 minutes per 5-second 480p clip**
- LTX-2 distilled: **about 2 minutes per 20 seconds of 720p**

Quality: MiniMax H3 is now competitive with top closed models on the Artificial Analysis arena. LTX-2.5 sits below Veo 3.1 and Kling 3.0.

### Cited Findings: developer-stated VRAM and length (WanGP-optimized)
All from the [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md):
- **MiniMax H3:** "5-6GB of VRAM only for 5s (124 frames) and 8-9GB of VRAM for 15s at 832x480." There are no step-distilled checkpoints at launch: "15-20 inference steps is a minimum." LoRA accelerators came later. The developer says high-res H3 "can be slow… a practical alternative is to generate at a lower resolution, such as 480p, and upscale."
- **LTX-2:**
  - At launch (v10.11, Jan 2026): runs "with as low as 10 GB of VRAM. If you have at least 24 GB of VRAM you will be able to generate 20s at 720p in a single window in only 2 minutes with the distilled model."
  - From v10.21: "10s at 720p… only 8GB of VRAM; 10s at 1080p with only 12 GB; 20s at 1080p with only 16 GB; 10s at Full 4k (3840x2176) with 24 GB." The same entry warns that "LTX-2 video is not for 4K, as 4K outputs may give you nightmares."
- **Hunyuan 1.5:** "less than 20 GB of VRAM to generate 12s (289 frames) at 720p."
- **Wan 2.2 Ovi 10 GB:** "only 6 GB of VRAM to generate 121 frames at 720p."
- **Bernini 14B (Wan 2.2 derived):** 81 frames needs 12 GB for v2v and 16 GB with reference frames.
- **Older Wan figures:**
  - Wan 2.1 with RIFLEx: ">10s of video at 720p with a RTX 4090 and 10s at 480p with less than 12GB."
  - On an RTX 2080Ti: "5s (81 frames, 15 steps) of Vace 1.3B with only 5GB and in only 6 minutes… 5s of t2v 14B in less than 10 minutes."
- **LTX-Video 0.9.8 (legacy):** "1800 frames (1 min of video!) in one go… distilled… only 5 minutes with a RTX 4090 (22 GB of VRAM)."
- **System RAM:**
  - Flux 2: "You will need at least 64 GB of RAM."
  - Out-of-memory crashes are reported by "users (with usually PC with less than 64 GB of RAM)" when using LoRAs with LTX-2.
  - Memory Profile 5 is the low-RAM option.
- The official docs now say the old fixed "6 GB / 12 GB / 20 GB" tiers are "not reliable." Resolution, frames, quantization, memory profile and sliding-window size all matter. — [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md)
- README headline: "run select models with as little as 6 GB of VRAM." It supports GTX 10XX and newer Nvidia cards and AMD RDNA 2–4. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Free VRAM trick: disabling GPU use in the web browser frees "between 1GB… and 5GB of VRAM." — [CHANGELOG v12.3](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)

### Cited Findings: third-party benchmarks
- **SaladCloud, Wan 2.1 T2V-14B, full precision, no WanGP optimizations:**

  | GPU | 5 s at 480p | 5 s at 720p |
  | --- | --- | --- |
  | H100 SXM | 85 s | 284 s |
  | A100 SXM | 170 s | 523 s |
  | RTX 4090 | 281 s | out of memory |
  | A40 | 501 s | 1,083 s |

  Source: [runaihome.com](https://runaihome.com/blog/wan-video-local-ai-gpu-guide-2026/), citing SaladCloud.
- **Community figures for optimized Wan 2.2** (same source):

  | Setup | Time per 5 s clip |
  | --- | --- |
  | RTX 4090, FP8, 720p | 2–4 min |
  | RTX 5060 Ti 16GB, 720p | 2–4 min |
  | RTX 4070 12GB, GGUF, 480p | 18–22 min |
  | RTX 3090, 640×480 | about 9–10 min (81 frames) |
  | RTX 4060 8GB, Wan 1.3B, 480p | 4–6 min |

  The same guide says 32 GB of system RAM is the minimum; "16 GB insufficient." — [runaihome.com](https://runaihome.com/blog/wan-video-local-ai-gpu-guide-2026/)
- **Wan 2.2 with the Lightning (LightX2V) 4-step LoRA:** drops a 480p 5 s clip "from around 40-50 minutes down to roughly 1-3 minutes on an RTX 4090." Without acceleration, 720p takes about 9 min per 5 s on a 4090. This is a secondary source summarized by search. — [MindStudio](https://www.mindstudio.ai/blog/what-is-wan-2-2-video-open-source)
- **Older Wan2GP explainer (Sept 2025):** "6GB cards can do 5s/480p, 12GB cards reach 8s/720p or 15s/480p, and 24GB cards stay entirely in VRAM for ~3× faster churn." This predates many optimizations, so treat it as dated. — [BrightCoding blog](https://www.blog.brightcoding.dev/2025/09/17/open-source-video-generation-for-low-vram-gpus-how-wan2gp-puts-cinematic-ai-in-reach-of-the-gpu-poor)

### Cited Findings: clip length and resolution
- The native window is model-dependent: about 5 s for Wan (81 frames at 16 fps), 15 s for H3 at 480p, and 20 s for LTX-2 at 720p/1080p.
- Longer output uses sliding windows. Frame count is `(Nb Windows − 1) × (Window Size − Overlap − Discard) + Window Size`.
- The docs warn: "The longer the video, the more likely quality is to degrade over time: artifacts can accumulate and character identity may progressively drift." The suggested mitigation is to pre-generate end-frame anchor images for each window.
- Sources: [docs/PROCESSING.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/PROCESSING.md); [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- 3K/4K output has to be enabled in Config. Kandinsky 5 / Magi-type models are "res picky" (for example, one model works only at 256p or 1080p). — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)

### Cited Findings: quality vs. closed tools
From the **Artificial Analysis Text-to-Video leaderboard (AA-Video-T2V v2.0)**, fetched 3 Oct 2026 — [Artificial Analysis](https://artificialanalysis.ai/video/leaderboard/text-to-video):

| Rank | Model | Elo | Status |
| --- | --- | --- | --- |
| 4 | MiniMax H3 (768p) | 1138 | open weights |
| 7 | Seedance 2.0 | 1118 | closed |
| 13 | Wan 2.7 | 1030 | closed |
| 15 | Kling 3.0 Omni 1080p Pro | 1016 | closed |
| 18 | Veo 3.1 | 966 | closed |
| 22 | LTX-2.5 Fast | 948 | open weights |
| 23 | LTX-2.5 Pro | 944 | open weights |

- **Caveats:**
  - The page summarizer also flagged "Wan 3.0" (#1, 1157) as open weights. That conflicts with multiple sources saying Wan 3.0 has no downloadable weights ([TheNextWeb](https://thenextweb.com/news/alibaba-wan3-video-model-after-share-sale); [Evolink](https://evolink.ai/blog/wan-3-0-release)). Treat it as closed.
  - An earlier snapshot in search snippets showed different rankings, with Seedance 2.0 and HappyHorse at the top (1211) and LTX-2.3 at about 976. The board moves fast.
- The developer pitches **JoyAI-Echo** (LTX-2.3 based) as "the closest thing to SeeDance 2 that you may find in the open source world" for multi-shot stories, and **MiniMax H3** as "a top-notch open-weight contender to Seedance 2." These are maintainer claims. — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- Sora is reportedly "being switched off," per a search snippet attributed to an industry article. This is unverified. — [techsy.io](https://techsy.io/en/blog/best-ai-video-models)

### Inferences
- **Rule of thumb for cloud use:** rent a 24–32 GB GPU (RTX 4090 / 5090 / L40S) with at least 64 GB of system RAM. Then most models fit in VRAM ("Profile 3") and run several times faster than offloaded low-VRAM profiles. Low-VRAM cleverness matters less when you rent.
- Arena Elo measures short single clips. For long-form b-roll, consistency and edit-ability matter as much as raw Elo.

### Gaps
- I found no rigorous, dated benchmarks of **WanGP specifically** for MiniMax H3 or LTX-2.5 on a 3060, 4060, 4090 or 5090. The developer's figures are mostly VRAM, not time, apart from the LTX-2 "20 s 720p in 2 min on 24 GB."
- I found no RTX 5090 timings for Wan 2.2 with Lightning LoRAs.
- I found no independent side-by-side of WanGP output vs. ComfyUI output for the same model. They should be identical in principle, since both run the same weights.

---

## 4. Running it with no GPU: cloud options, prices, cost per finished minute, and free tiers

### Takeaway
Yes, WanGP runs well on rented cloud GPUs. The cheapest practical setup is a **RunPod or Vast.ai RTX 4090 (about $0.29–$0.74/hr) or RTX 5090 (about $0.69–$0.99/hr)** using the community Docker image, a RunPod template, or a manual git clone, with a persistent volume for the large model files.

Estimated cost per *finished* minute of video, including retries, is roughly **$0.10–$0.50 with LTX-2 distilled** and **$0.60–$4 with Wan 2.2 / H3**, depending on resolution and how many takes you discard. That is far below closed APIs (Wan 3.0: $12 per minute of output).

Free options are marginal:
- **Colab free:** its FAQ bans "web UI" use, which WanGP is.
- **Kaggle:** T4/P100, about 30 hours a week; possible but slow and VRAM-limited.
- **Lightning AI / Modal free credits:** good for testing only.

### Cited Findings: cloud GPU pricing
**RunPod**, prices updated 27 Sep 2026 — [RunPod pricing](https://www.runpod.io/gpu-cloud/pricing):

| GPU | Community Cloud | Secure Cloud |
| --- | --- | --- |
| RTX 3090 | $0.22/hr | $0.50/hr |
| **RTX 4090** | **$0.34/hr** | **$0.74/hr** |
| **RTX 5090** | **$0.69/hr** | **$0.99/hr** |
| L4 | $0.44/hr | $0.49/hr |
| L40S | $0.79/hr | $1.09/hr |
| A40 | $0.35/hr | $0.49/hr |
| RTX A6000 | $0.33/hr | $0.53/hr |
| A100 PCIe | $1.19/hr | $1.59/hr |
| H100 PCIe | $1.99/hr | $2.89/hr |

- RunPod storage: network storage $0.07/GB/month (under 1 TB), container and volume disk $0.10/GB/month. — [RunPod pricing](https://www.runpod.io/gpu-cloud/pricing)
- Billing is per second. — [diyai.io](https://diyai.io/ai-tools/hosting/runpod-pricing/)
- **Vast.ai RTX 4090:** from about $0.29–$0.31/hr on-demand, about $0.10/hr interruptible, typically $0.25–$0.40/hr, as of Aug 2026. — [computeprices.com](https://computeprices.com/providers/vast/gpus/rtx4090)

### Cited Findings: ready-made deployment
- Community Docker image `thankfulcarp/wan2gp-docker` has a RunPod template (`:runpod-ssh` tag), port 7860, and env-var paths (W2GP_MODELS, W2GP_LORAS, W2GP_OUTPUTS, W2GP_SETTINGS) for persistent volumes. Caveats: "Tested only on linux," CUDA 12.4, and SageAttention 2 compiled only for RTX 30XX/40XX, so it may not be optimal for RTX 50XX. It was updated about 19 days before 3 Oct 2026. — [Docker Hub](https://hub.docker.com/r/thankfulcarp/wan2gp-docker)
- The official repo ships `run-docker-cuda-deb.sh`, which auto-detects the GPU and builds an image with CUDA 12.4.1 / PyTorch 2.6.0 / SageAttention, but that Docker stack is older than the recommended manual stack (PyTorch 2.10 / CUDA 13). — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Remote access flags: `--listen`, `--server-port`, and `--share` (a public Gradio link). v13.10 added "Access Anywhere" multi-device sync, with a VPN recommended, and docs/AUTHENTICATION.md covers authentication and reverse proxies. — [docs/CLI.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CLI.md); [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- Headless batch: build a queue in the UI, save it, then run `python wgp.py --process my_queue.zip`. This suits spinning up a pod, rendering a batch and shutting it down. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- RunPod also publishes ComfyUI-based Wan 2.2 templates. Those are an alternative app, not WanGP. — [RunPod guide](https://www.runpod.io/articles/guides/comfyui-wan-2-2)

### Cited Findings: free tiers
- **Colab:**
  - The official FAQ lists, for the free tier without a paid balance, the disallowed "remote control such as SSH shells, remote desktops" and "bypassing the notebook UI to interact primarily via a web UI." The free tier also has a 12-hour session maximum. — [Colab FAQ](https://research.google.com/colaboratory/faq.html)
  - Colab's head said the Gradio restriction was about prioritizing free-tier compute. — [Decrypt](https://decrypt.co/197428/google-colab-stable-diffusion-web-ui-ban)
- **Colab notebook for WanGP:** a community notebook exists, Square-Zero-Labs "Wan2GP-on-Colab," updated 7 Aug 2026. Its own note says free T4 (about 15 GB VRAM) "is too small for most Wan2GP checkpoints." It recommends only "Wan 2.2 TextImage2Video 5B FastWan" at 480p and suggests paid A100/L4 for more. — [GitHub: Wan2GP-on-Colab](https://github.com/Square-Zero-Labs/Wan2GP-on-Colab)
- **Kaggle:** T4 or P100, 12-hour sessions, a weekly GPU quota of "commonly ~30 hours, varies by account," and no documented web-UI restriction. — [gpuperhour.com, 21 Sep 2026](https://gpuperhour.com/blog/free-cloud-gpus-and-credits). That page lists the P100 as 24 GB, which is wrong; the P100 is 16 GB.
- **Lightning AI:** free tier of about 15 monthly credits, which is "≈80 GPU hours on spot," with 4-hour restarts. The amounts are unconfirmed per gpuperhour. — [search summary of usagepricing / gmicloud](https://gmicloud.ai/blog/where-can-i-get-free-gpu-cloud-trials-in-2026-a-complete-guide); [gpuperhour.com](https://gpuperhour.com/blog/free-cloud-gpus-and-credits)
- **Modal:** Starter plan gives "$30 / month free compute." It is serverless and code-first, not notebook-style. — [Modal pricing](https://modal.com/pricing); [gpuperhour.com](https://gpuperhour.com/blog/free-cloud-gpus-and-credits)
- **Paperspace free:** M4000 8 GB, 6-hour sessions. **SageMaker Studio Lab:** closed to new customers. — [gpuperhour.com](https://gpuperhour.com/blog/free-cloud-gpus-and-credits)
- **Closed API comparison:** Wan 3.0 costs $0.20 per output second at 1080p, which is $12 per minute of output, with no retries counted. — [TheNextWeb](https://thenextweb.com/news/alibaba-wan3-video-model-after-share-sale)

### Inferences: cost per finished minute
These are my estimates built from the cited speeds and prices. They are not measured.

**Assumptions:**
- 1 finished minute is 12 Wan clips of 5 s, or 3 LTX-2 windows of 20 s.
- You keep about 1 in 3 generations, which is a guess. Real keep rates vary widely by prompt skill and use of I2V.
- Pods run on RunPod Community Cloud (4090 at $0.34/hr, 5090 at $0.69/hr) or Secure Cloud (4090 at $0.74/hr).

| Workflow | GPU time per finished minute | RunPod cost (Community to Secure) |
| --- | --- | --- |
| Wan 2.2 14B + 4-step Lightning, 480p, about 2 min/clip on 4090 | 36 × 2 min ≈ 1.2 h | about $0.40–$0.90 |
| Wan 2.2 14B accelerated, 720p, about 3 min/clip on 4090 | 36 × 3 min ≈ 1.8 h | about $0.60–$1.35 |
| Wan 2.2 14B un-accelerated, 720p, about 9 min/clip on 4090 | 36 × 9 min ≈ 5.4 h | about $1.85–$4.00 |
| LTX-2 distilled, 720p, about 2 min per 20 s on 24 GB (developer claim) | 9 × 2 min ≈ 18 min | about $0.10–$0.25 |
| MiniMax H3 (no reliable timing found) | probably slower than Wan with Lightning, since it needs 15–20 steps without accelerators | likely $1–$5; unverified |

**Fixed overheads:**
- Pod boot, setup and model downloads take tens of minutes on first run. I have no source for model sizes.
- A 100–300 GB network volume would cost about $7–$21 per month at $0.07/GB, by my arithmetic from the RunPod rates.
- Idle time while you write prompts is billed, so stop pods when not generating, or use the headless queue.

For a YouTube channel publishing, say, 10 minutes of AI b-roll per week, GPU spend is plausibly **$5–$40 per week** plus about $10–$20 per month of storage. Debugging time is the bigger cost.

**Free path verdict:**
- Colab free is effectively ruled out by Colab's own policy, since WanGP is a Gradio web UI, and by the T4 VRAM limits.
- Kaggle is a gray-zone possibility for small models (Wan 2.2 5B at 480p) through a tunnel or the `--share` link. Kaggle's terms on that were not checked. It would be slow, with fp16-only T4/P100 cards that lack modern attention kernels.
- Lightning AI or Modal free credits are the most realistic free way to *try* WanGP.

### Gaps
- I did not verify Kaggle's terms on running a web UI or tunnel, or Lightning AI's exact current credit amount and GPU types.
- I found no Pinokio cloud template. Pinokio is a local desktop installer, and I found no evidence of hosted Pinokio GPU templates.
- I found no measured WanGP pod cold-start or download times, and no model disk sizes.
- I did not fetch Vast.ai RTX 5090 or L40S pricing.

---

## 5. Installation methods and common pitfalls

### Takeaway
There are five installation methods:
1. One-click `.bat`/`.sh` scripts, which install acceleration kernels too
2. Pinokio, where the README recommends the community script by "Morpheus" over the official one
3. The Wan2GP Desktop launcher
4. Manual conda (Python 3.11, PyTorch 2.10, CUDA 13)
5. Docker

Pitfalls cluster around Triton/SageAttention version matching, system RAM exhaustion, and docs that lag the code.

### Cited Findings
- **One-click scripts:** `scripts/install.sh` / `install.bat` "install WanGP but also best acceleration kernels (Triton, Sage, Flash, GGuf, Lightx2v, Nunchaku) available for your config." There are also `run`, `update` (Update vs. Upgrade) and `manage` scripts for switching environments. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Pinokio:** "It is recommended to use in Pinokio the Community Scripts *wan2gp* or *wan2gp-amd* by **Morpheus** rather than the official Pinokio install." — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Manual install (RTX 20XX–50XX):** `conda create -n wan2gp python=3.11.14`, then `pip install torch==2.10.0 … --index-url …/cu130`, then `pip install -r requirements.txt`, then `python wgp.py`. GTX 10XX uses Python 3.10.9 with torch 2.7.1/cu128. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Triton and SageAttention pitfalls:**
  - Triton must match the GPU and PyTorch combination: 3.6.x with PyTorch 2.10 on RTX 30XX+, 3.2.x on RTX 20XX, 3.7.x on AMD.
  - "Update alone does not change it."
  - Triton 3.2 on RTX 30XX+ can crash YuE2's INT8 kernels.
  - RTX 50XX needs PyTorch 2.10 / Triton 3.6 for INT8 and NV FP4.
  - SageAttention: 1.0.6 on RTX 20XX, 2.2.0 on RTX 30XX+, SDPA on GTX 10XX.
  - Source: [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Upgrades:** moving from Python 3.10 / PyTorch 2.7.1 to 3.11 / 2.10 requires a new conda env and reinstalling Sage, Triton and Flash. For git conflicts, use `git fetch origin && git reset --hard origin/main`. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Memory profiles:** `--profile 1–5` (default 4). Profile 3 preloads the model in VRAM and is fastest with high VRAM. Profile 5 is minimal-RAM. For RAM out-of-memory, use `--perc-reserved-mem-max 0.3` and/or enable a swap file. — [docs/TROUBLESHOOTING.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/TROUBLESHOOTING.md); [docs/CLI.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CLI.md)
- **LoRA out-of-memory:** on PCs with under 64 GB RAM, out-of-memory errors when starting a generation with LoRAs come from failed "pinning" of LoRAs to reserved RAM. An experimental auto-recovery was added. — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Docker:** the community image is compiled only for RTX 30/40 with CUDA 12.4. The official Docker script uses PyTorch 2.6, which is older than the recommended native stack. — [Docker Hub](https://hub.docker.com/r/thankfulcarp/wan2gp-docker); [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Stale docs:** GETTING_STARTED.md still says "Ensure Python 3.10.9 is used," while elsewhere in the same doc and in the README the recommended version is 3.11.14. — [docs/GETTING_STARTED.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/GETTING_STARTED.md)
- **Unofficial copies:** the README warns against them. Unofficial HF Space mirrors of WanGP docs exist, such as `attong39/Wan2GP` and `USF00/Wan2GP_Demo`. — [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md); [HF mirror example](https://huggingface.co/spaces/attong39/Wan2GP/blob/main/docs/GETTING_STARTED.md)

### Inferences
- On a cloud pod, the most robust path is a fresh RunPod PyTorch/CUDA template. Clone the repo, run `scripts/install.sh` (auto mode), put `models/` and `loras/` on a network volume, and launch with `--listen`.
- Use the community Docker image only on 30/40-series pods.
- Expect each major WanGP update to occasionally need an "Upgrade" of kernels.

### Gaps
- I did not read GitHub issues directly, because API access was blocked. I could not quantify the most common open bugs.

---

## 6. Honest limitations for YouTube production

### Takeaway
WanGP is a powerful, free front-end, but it inherits open-model limitations:
- clips are short per window (5–20 s)
- long videos drift in identity and accumulate artifacts
- character consistency across shots needs extra workflow (reference/identity models, LoRAs, anchor frames)
- in-video text is weak unless rendered by image models
- iteration takes time

For b-roll these are manageable. Narrative Shorts with recurring characters need more effort.

### Cited Findings
- **Drift and degradation:** "The longer the video, the more likely quality is to degrade over time: artifacts can accumulate and character identity may progressively drift." Repeated continuation "can slightly degrade quality at each stage" through lossy re-encoding. Sliding-window overlap can propagate blur. — [docs/PROCESSING.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/PROCESSING.md)
- **Consistency tooling:**
  - JoyAI-Echo multi-shot "memories"
  - LTX-2.3 MSR V2 (2–5 subject references)
  - H3 Ref2VA references that persist across windows
  - Lynx identity face replacement
  - Krea 2 / Qwen Image Edit identity editing for generating consistent first frames
  - Sources: [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md); [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Resolution and quality quirks:**
  - LTX-2 at 4K "may give you nightmares."
  - Some models only work well at specific resolutions ("res picky").
  - Ghosting was observed on a 540p variant.
  - Source: [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Speed:** H3 has no step-distilled checkpoint at launch and needs at least 15–20 steps, so high-res generation is slow. The developer suggests generating at 480p and upscaling. — [CHANGELOG](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/CHANGELOG.md)
- **Text rendering:** the docs direct users to image models for typography: Ideogram 4 for "layout, typography," Qwen Image for "rendering longer text," and SenseNova / Ming Image for infographics. Video models are not positioned as text renderers. — [docs/MODELS.md](https://github.com/deepbeepmeep/Wan2GP/blob/main/docs/MODELS.md); [README](https://github.com/deepbeepmeep/Wan2GP/blob/main/README.md)
- **Quality gap:** LTX-2.5 (about 945 Elo) sits about 70–200 Elo below top closed models, while MiniMax H3 (1138) is near the top of the board. — [Artificial Analysis](https://artificialanalysis.ai/video/leaderboard/text-to-video)

### Inferences
- **Long-form b-roll** (landscapes, abstract or atmospheric shots, product-free scenes) is the sweet spot. Generate 5–15 s shots and cut between them; do not rely on 60 s continuous takes.
- For text overlays, add text in the video editor rather than in generation.
- **Shorts** with a recurring mascot or presenter:
  1. Build a character reference set with Krea 2 / Qwen Edit.
  2. Use I2V or Ref2VA / InfiniteTalk for each shot.
  3. Optionally train or obtain a character LoRA. LoRA training itself is not a WanGP feature and is not covered here.
- **Time cost:** the dominant cost is human iteration (prompting, rerolls, review), not GPU dollars.

### Gaps
- I found no systematic study of WanGP output artifacts (hands, physics, flicker) per model.
- I found no data on YouTube's treatment of AI b-roll (monetization, disclosure). That is out of scope here but relevant to the overall plan.
