# Licensing of Wan2GP-usable models (A) and a budget faceless-YouTube production workflow (B)

Research date: 2026-10-03. Several "newest" facts below (Wan2GP v13.x model list, MiniMax H3 open weights, YouTube 2026 enforcement dates) come from 2026 sources I could not cross-check against a second primary source; they are flagged where relevant.

## A1. What license is Wan2GP itself under, and does it allow monetizing outputs?

### Takeaway
Wan2GP is NOT MIT/Apache. It uses the custom "WanGP Community License 2.0". It lets you use, publish and sell outputs for any lawful purpose. A "Made with WanGP" credit is only required for "Direct Output Sales", meaning selling the clips themselves. What it restricts is reselling or hosting the *software* (paid SaaS/API, white-labelling). Each model's own license still applies on top of it.

### Cited Findings
- The repo describes itself as "free to use locally" and says the official project "will never ask you to pay a license fee, subscription, or donation to run WanGP on your own computer (see the license for terms)". Latest release noted: WanGP v13.141, released Sept 29, 2026 — [Wan2GP GitHub README](https://github.com/deepbeepmeep/Wan2GP)
- License is "WanGP Community License 2.0". Outputs clause: "you may use, reproduce, publish, perform, display, distribute, sell, and license Outputs for any lawful purpose". Credit is required only for Direct Output Sales, and wording such as "Made with WanGP" in the usual credits location is enough — [Wan2GP LICENSE.txt](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/LICENSE.txt)
- "Direct Output Sale" = "selling, licensing, or otherwise charging separate consideration for an Output, or a collection of Outputs, where the principal value of the transaction is the Output itself" — [Wan2GP LICENSE.txt](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/LICENSE.txt)
- The license prohibits, without a commercial license, "offering the Software through a paid, metered, sponsored, ad-supported, subscription, hosted, managed, API, SaaS" arrangement, plus selling or white-labelling WanGP. Internal business use and client work that uses WanGP as an internal tool are allowed. The license does not mention YouTube or ad-supported *outputs* — [Wan2GP LICENSE.txt](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/LICENSE.txt)
- "Third-Party Materials remain licensed under their own terms." The WanGP license does not narrow the rights the model licenses give — [Wan2GP LICENSE.txt](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/LICENSE.txt)
- Model families listed in the current README:
  - Video: Wan 2.1/2.2, MiniMax H3, LTX-2/2.3/2.5, Hunyuan Video 1/1.5, LongCat, Kandinsky, LTXV, MagiHuman
  - Image: Krea 2, Qwen Image (incl. 2.1), Z-Image, Flux 1/2, SenseNova, Ideogram 4, HiDream, Ming Image
  - Audio/TTS/music: Qwen3 TTS, MiniMax H3 Voice Clone, Ace Step, OmniVoice, Index TTS2/2.5, KugelAudio, HeartMula, Chatterbox, Minimax Music, Stable Audio 3, YuE2, AuK Speech
  - Source: [Wan2GP GitHub README](https://github.com/deepbeepmeep/Wan2GP)

### Inferences
- A YouTube video monetized through ads is not a "Direct Output Sale". Viewers are not paying for the clip itself. So WanGP's credit requirement most likely does not apply to ad-monetized YouTube videos. Adding "Made with WanGP" to the description costs nothing and removes the doubt.
- The "ad-supported" prohibition is about offering the *software* to others, for example running a website where people generate videos. It does not cover publishing videos you made.

### Gaps
- No version date was found in the license text. I also did not confirm whether earlier Wan2GP releases used a different license, such as plain Apache/MIT.

## A2. Model-by-model licensing table (can outputs be monetized on YouTube?)

### Takeaway
For an India-based creator, the safest video models are **Wan 2.1/2.2 (Apache 2.0)** and **LTX-2.x (free under $10M annual revenue)**. Hunyuan Video and MiniMax H3 are usable from India but carry territory exclusions, use restrictions and labelling or attribution terms. Wan 2.5/2.6/2.7 are not open weights at all, so they are API-only and outside Wan2GP. Flux "dev" models are non-commercial *for the weights*, but their outputs can be used commercially. Watch for non-commercial *weights* in the TTS models: F5-TTS, XTTS-v2, and IndexTTS2 (disputed).

### Cited Findings

**Table: model families in Wan2GP**

| Model family | License | Monetize outputs on YouTube? | Key restrictions | Source |
|---|---|---|---|---|
| Wan 2.1 / 2.2 (Alibaba) | Apache 2.0 | Yes | None material. Wan 2.2 released July 28, 2025 | [howaiworks.ai](https://howaiworks.ai/blog/alibaba-wan-open-weights-stopped-at-2-2.md) |
| Wan 2.5 / 2.6 / 2.7 | Closed, API-only (Alibaba Cloud Bailian/Model Studio, Tongyi Wanxiang) | Per API ToS. Not runnable in Wan2GP | No open weights. "Alibaba's Wan open-weight line ends at Wan 2.2" | [howaiworks.ai](https://howaiworks.ai/blog/alibaba-wan-open-weights-stopped-at-2-2.md); [aiwiki Wan 2.5](https://aiwiki.ai/wiki/wan_2_5) |
| Hunyuan Video 1 / 1.5 (Tencent) | Tencent Hunyuan Community License | Yes, in India/US | "DOES NOT APPLY IN THE EUROPEAN UNION, UNITED KINGDOM AND SOUTH KOREA"; >100M MAU must request a license; Tencent "claims no rights in Outputs"; outputs may not be used to improve other AI models; must "expressly and conspicuously" identify machine-generated content when disseminated | [HunyuanVideo-1.5 LICENSE](https://huggingface.co/tencent/HunyuanVideo-1.5/blob/main/LICENSE) |
| LTX-2 / 2.3 (Lightricks) | LTX-2 Community License Agreement | Yes, below $10M revenue | Entities with $10,000,000+ annual revenue need a paid license; "Licensor claims no rights in the Output"; 20 prohibited uses (harmful misinformation, impersonation without consent, medical advice, etc.); sanctions/export rules; no territory exclusion | [LTX-2 LICENSE](https://huggingface.co/Lightricks/LTX-2/blob/main/LICENSE); [LTX-2 model card](https://huggingface.co/Lightricks/LTX-2) |
| MiniMax H3 (open weights Aug 3, 2026) | MiniMax H3 Community License (not OSI open source) | Yes in India (needs verification of output terms) | Territory excludes **EU, UK, South Korea and the USA**; revenue >US$20M/yr needs written authorization; commercial use reportedly requires displaying "MiniMax H3" in product UI; downloadable weights cap at 768p | [NYU Shanghai RITS](https://rits.shanghai.nyu.edu/ai/minimax-ships-h3-weights-with-the-us-and-eu-excluded/); [atlascloud](https://www.atlascloud.ai/blog/guides/minimax-h3-open-source-weights) |
| FLUX.1 [dev] / Kontext [dev] | FLUX.1 [dev] Non-Commercial License | **Outputs yes**; weights non-commercial | "You may use Output for any purpose (including for commercial purposes)"; may not use outputs to train competing models; must include AI disclosure "to the extent required under applicable law" | [FLUX.1-dev LICENSE](https://huggingface.co/black-forest-labs/FLUX.1-dev/blob/main/LICENSE.md) |
| FLUX.1 [schnell] | Apache 2.0 | Yes | None | [invideo license matrix](https://invideo.io/blog/open-source-image-models-licenses/) |
| FLUX.2 klein-4B | Apache 2.0 | Yes | "the only unrestricted checkpoint in the Flux 2 family" | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| FLUX.2 dev (32B) / klein-9B | FLUX Non-Commercial | Outputs yes | Commercial *deployment* needs a paid BFL license | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| Qwen-Image (incl. 2512, Edit ≤2511, Layered) | Apache 2.0 | Yes | Source qualifies this as the "pre-2026 line". Qwen Image 2.1's license is unverified | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| Z-Image (Turbo & base) | Apache 2.0 | Yes | None | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| Krea 2 (K2 Raw/Turbo, June 23, 2026) | "Permissive" per Krea, verify card | Likely | Unverified | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| Ideogram 4 (open weights) | Gated non-commercial | **No** (per source) | "gated non-commercial with no revenue threshold" | [invideo](https://invideo.io/blog/open-source-image-models-licenses/) |
| Chatterbox / Chatterbox Multilingual v3 (Resemble AI) | MIT | Yes | Outputs carry an embedded (Perth) watermark. Hindi supported as a Language Pack | [Resemble AI](https://www.resemble.ai/resources/chatterbox-multilingual-v3-tts-with-embedded-watermarking-for-25-languages) |
| Qwen3-TTS (Alibaba) | Apache 2.0 | Yes | 10 languages (zh, en, ja, ko, de, fr, ru, pt, es, it). **No Hindi** | [QwenLM/Qwen3-TTS GitHub](https://github.com/QwenLM/Qwen3-TTS) |
| IndexTTS2 / 2.5 (Bilibili) | Code Apache 2.0; weights under "bilibili Model Use License"/INDEX_MODEL_LICENSE | **Disputed** | One source says commercial use is free below 100M MAU / RMB 1B revenue. Others say DISCLAIMER.md/INDEX_MODEL_LICENSE require prior written authorization and that the weights are NonCommercial | [index-tts issue #228](https://github.com/index-tts/index-tts/issues/228); [mlx-indextts2-swift](https://github.com/xocialize/mlx-indextts2-swift) |

**TTS models outside Wan2GP that came up in the brief**

| Model | License | Monetizable? | Hindi? | Source |
|---|---|---|---|---|
| Kokoro-82M | Apache 2.0 | Yes | Yes (Hindi among en/ja/zh/fr/it/pt/es/hi) | [PyPI kokoro](https://pypi.org/project/kokoro/); [Replicate](https://replicate.com/jaaari/kokoro-82m) |
| F5-TTS | Code MIT; **weights CC-BY-NC-4.0** (Emilia training data) | **No** with official weights | — | [localaimaster F5 guide](https://localaimaster.com/blog/f5-tts-setup-guide) |
| XTTS-v2 (Coqui) | Coqui Public Model License 1.0 (non-commercial) | **No**. Coqui shut down Dec 2023, so no commercial license can be bought | Yes | [localaimaster XTTS](https://localaimaster.com/blog/xtts-coqui-commercial-license) |
| Indic Parler-TTS (AI4Bharat) | Apache 2.0 | Yes | Yes (21 languages incl. Hindi, many Indic) | [HF ai4bharat/indic-parler-tts](https://huggingface.co/ai4bharat/indic-parler-tts) |
| Sarvam Bulbul v3 (API, paid) | Commercial API | Yes | Yes, Indian-made. ~₹3 per 1,000 chars (also reported as ₹30/10k chars in beta, Aug 2026) | [invideo Bulbul](https://invideo.io/blog/sarvam-bulbul-indian-tts/) |
| ElevenLabs (paid) | Commercial license on paid plans | Yes (Starter+) | Yes | [smallest.ai pricing](https://smallest.ai/blog/elevenlabs-pricing-plans-cost-what-you-get-in-2026) |

### Inferences
- Territory clauses (Hunyuan, MiniMax H3) are written about where the *licensee* is located and uses the model. An India-based creator falls inside the permitted territory. Publishing the resulting video on YouTube, where US/EU viewers watch it, is distribution of *outputs*, not use of the model in those territories. These licenses don't say explicitly how output distribution into excluded regions is treated. For a creator with US/EU collaborators or plans to relocate, Wan 2.2 or LTX-2 is the lower-risk default.
- Hunyuan's "conspicuously identify machine-generated" rule and Flux's "disclosure to the extent required by law" can both be met with YouTube's "altered or synthetic content" toggle plus a line in the description.
- Recommended "clean stack" for a monetized channel: Wan 2.2 (video), LTX-2.x (fast video, with native audio), Z-Image / Qwen-Image / FLUX.1 schnell / FLUX.2 klein-4B (images), Kokoro or Chatterbox (English voice), Indic Parler-TTS, Chatterbox Hindi pack or Kokoro Hindi (Hindi voice). Avoid F5-TTS, XTTS-v2 and Ideogram 4 weights. Treat IndexTTS2 as risky until bilibili clarifies.

### Gaps
- I did not verify the licenses of LongCat, Kandinsky, MagiHuman, SenseNova, HiDream, Ming Image, Ace Step, OmniVoice, KugelAudio, HeartMula, YuE2, Stable Audio 3, Minimax Music or AuK Speech. A newer Wan2GP release could add more. Stable Audio models have historically used Stability AI community licenses with revenue thresholds; check the current card.
- I did not read the full MiniMax H3 LICENSE. Whether its "display MiniMax H3 in your product UI" term applies to video outputs, as opposed to products and apps, is unverified.
- LTX-2.3 / 2.5 licenses were assumed to match LTX-2. Only the LTX-2 license file was read.
- The Wan 2.1/2.2 Apache 2.0 status is cited from a secondary registry blog, not fetched directly from the Wan-AI HF model card in this session.

## A3. Who owns AI-generated output, and what about copyright registration and Content ID?

### Takeaway
Model licensors (Tencent, Lightricks, BFL, the WanGP license) claim no rights in outputs. In the US, though, purely AI-generated material is not copyrightable: the Copyright Office's Jan 2025 Part 2 report says so, and the Supreme Court denied cert in *Thaler v. Perlmutter* on March 2, 2026. Raw AI clips are therefore effectively free for others to reuse. The protectable layer is the human-authored script, voice direction, edit, selection and arrangement.

### Cited Findings
- USCO Part 2 report (Jan 29, 2025): "copyright protection in the United States requires human authorship". Works created solely by AI without human intervention are not eligible — [Pressbooks summary of USCO Part 2](https://pressbooks.pub/aicopyrightanddataprivacyineducation/?p=312); [JD Supra](https://www.jdsupra.com/topics/authorship/machine-learning/copyright-registration/)
- The Supreme Court denied certiorari in *Thaler v. Perlmutter* on March 2, 2026, leaving the D.C. Circuit's human-authorship requirement in place — [Munck Wilson](https://www.munckwilson.com/news-insights/supreme-court-denies-certiorari-in-thaler-v-perlmutter-human-authorship-still-required/); [Penningtons](https://www.penningtonslaw.com/insights/ai-art-and-global-approaches-to-copyright-law-us-supreme-court-declines-to-review-the-case-of-thaler-v-perlmutter/)
- Licensor stances: Tencent "claims no rights in Outputs" — [Hunyuan LICENSE](https://huggingface.co/tencent/HunyuanVideo-1.5/blob/main/LICENSE). Lightricks "claims no rights in the Output you generate" — [LTX-2 LICENSE](https://huggingface.co/Lightricks/LTX-2/blob/main/LICENSE). WanGP lets users sell and license outputs — [WanGP LICENSE](https://raw.githubusercontent.com/deepbeepmeep/Wan2GP/main/LICENSE.txt)

### Inferences
- Content ID consequence: a creator could struggle to enforce claims over purely AI-generated b-roll, and someone else could reuse the same clips. Original human narration and script, plus a substantial edit, give the video as a whole a stronger compilation-level claim.
- Content ID eligibility requires exclusive rights. Channels built mostly on AI visuals are unlikely to qualify or benefit. Small creators don't get Content ID access anyway; they use the Copyright Match Tool. This is general knowledge and was not fetched in this session.
- India's position on AI-authorship is unsettled. No source was retrieved.

### Gaps
- I found no India-specific (Copyright Act 1957) guidance on AI-generated works in this session.
- I did not verify YouTube Content ID's current eligibility rules for AI-generated material.

## B1. YouTube policy constraints on faceless AI channels (inauthentic content, disclosure)

### Takeaway
On July 15, 2025, YouTube renamed "repetitious content" to "inauthentic content". The policy explicitly targets "AI-generated content made with generic or unoriginal templates giving the impression of mass production". AI as a tool remains monetizable when the content is original. AI personas posing as human experts on health, legal, finance or politics cannot be monetized. Reported 2026 enforcement adds auto-labelling of undisclosed photorealistic AI content.

### Cited Findings
- Official policy (updated July 15, 2025): content must "not be mass-produced, generic, repetitive, or manipulative". The listed example is "AI-generated content made with generic or unoriginal templates giving the impression of mass production". AI is allowed when it "demonstrate[s] your creative vision and provide[s] educational or entertainment value". AI personas presenting as human experts on health, legal, financial or political topics are ineligible — [YouTube Help: channel monetization policies](https://support.google.com/youtube/answer/1311392?hl=en)
- Reused content: downloaded or copied content "without any substantive modifications" is not monetizable — [YouTube Help](https://support.google.com/youtube/answer/1311392?hl=en)
- Reported timeline (secondary aggregator):
  - January 2026: an enforcement wave against channels with "synthetic narration, templated thumbnails, stock-footage loops, and unsustainable upload speeds".
  - May 27, 2026: YouTube began auto-detecting and labelling undisclosed photorealistic AI content.
  - Aug 10, 2026: YPP changes announced, effective Feb 1, 2027, reportedly including 8,000 watch hours for new channels and a 20M Shorts-view threshold.
  - Source: [AIR Media-Tech timeline](https://air.io/en/monetization/youtube-monetization-policy-changes-2026-a-complete-dated-timeline). **Unverified against official YouTube sources; treat with caution.**
- Conflicting date: one aggregator says the inauthentic-content rename was effective July 15, **2026** — [tubetomp4](https://tubetomp4.it.com/blog/youtube-ai-content-crackdown-2026/). The official help page says **2025**, and that date should be used.
- Highest-risk patterns per practitioners: TTS-narrated stock-footage videos, undisclosed voice cloning, and 10+ near-identical uploads per day. "Faceless does not automatically mean inauthentic" — [tubetomp4](https://tubetomp4.it.com/blog/youtube-ai-content-crackdown-2026/); [OutlierKit](https://outlierkit.com/resources/faceless-youtube-channel-demonetized/)

### Inferences
- The budget plan should vary formats and keep a human-edited script with a distinct point of view and sourced facts. Avoid one fixed template, voice and stock loop for every video. Use the AI-disclosure toggle for photorealistic scenes.
- Do not build "AI doctor / AI financial advisor" personas, which are explicitly demonetized.

### Gaps
- I could not find the official YouTube announcement for the reported Aug 2026 / Feb 2027 YPP threshold changes. These need verification before they are put in the final report.

## B2. Scriptwriting with LLMs while staying "authentic"

### Takeaway
The YouTube policy text itself supplies the test: "original, authentic insights or perspective". Practical guidance is to use LLMs for research scaffolding and drafts, then add a human angle, fact-check against primary sources, and rewrite in your own voice.

### Cited Findings
- YouTube flags content produced via templates without the creator's "original, authentic insights or perspective" — [YouTube Help](https://support.google.com/youtube/answer/1311392?hl=en)
- "AI scripts checked and re-voiced by humans" are cited as remaining monetizable — [tubetomp4](https://tubetomp4.it.com/blog/youtube-ai-content-crackdown-2026/)

### Inferences
- Suggested process (practitioner norm, not sourced):
  1. Keyword/idea research with free tools: YouTube search autocomplete, Google Trends (with an India filter), the YouTube Studio "Research" tab, and the free tiers of vidIQ/TubeBuddy.
  2. Outline in a free LLM tier (ChatGPT, Gemini, Claude).
  3. Collect 3–5 primary sources and fact-check every number.
  4. Add a personal take, original analogies and a hook.
  5. Read aloud and edit.
- An 8–12 minute script is about 1,200–1,800 words, at roughly 150 wpm for English narration.

### Gaps
- No sourced pricing for vidIQ/TubeBuddy or LLM subscriptions was retrieved in this session.

## B3. AI voice options (English and Hindi): quality, license, cost

### Takeaway
Free and commercially safe options:
- **English:** Kokoro (Apache, CPU-friendly) or Chatterbox (MIT, cloning, watermark).
- **Hindi:** Indic Parler-TTS (Apache), Kokoro's Hindi voices, or Chatterbox's Hindi pack.

Paid options are ElevenLabs (Starter ~$5–6/mo with commercial rights; Creator $22/mo) and, for the best value in Hindi, Sarvam Bulbul v3 at ~₹3 per 1,000 characters. Avoid F5-TTS and XTTS-v2 for monetized work.

### Cited Findings
- ElevenLabs: Starter ($5–6/mo) gives 30,000 credits/month, a commercial license and 2 instant voice clones. Creator ($22/mo) gives 121,000 credits and professional voice cloning. Free tier: 10,000 credits, no commercial license — [smallest.ai](https://smallest.ai/blog/elevenlabs-pricing-plans-cost-what-you-get-in-2026); [eesel.ai](https://eesel.ai/blog/elevenlabs-pricing)
- Sarvam Bulbul v3: ₹3.00 per 1,000 characters (earlier reported as ₹30 per 10k chars in beta, Aug 2026) — [invideo Bulbul explainer](https://invideo.io/blog/sarvam-bulbul-indian-tts/)
- Indic Parler-TTS is Apache 2.0. Practitioners use it in Hindi YouTube Shorts pipelines — [HF card](https://huggingface.co/ai4bharat/indic-parler-tts); [GitHub devotional-shorts pipeline](https://github.com/0codespace/devotional-shorts-indic-parler-tts)
- Kokoro: Apache 2.0, supports Hindi — [PyPI](https://pypi.org/project/kokoro/)
- Chatterbox Multilingual v3: MIT, 25 languages incl. Hindi language pack, embedded watermarking — [Resemble AI](https://www.resemble.ai/resources/chatterbox-multilingual-v3-tts-with-embedded-watermarking-for-25-languages)
- F5-TTS weights are CC-BY-NC-4.0 and XTTS-v2 is CPML non-commercial, so both are unsuitable for monetized channels — [localaimaster F5](https://localaimaster.com/blog/f5-tts-setup-guide); [localaimaster XTTS](https://localaimaster.com/blog/xtts-coqui-commercial-license)
- Qwen3-TTS (Apache) covers 10 languages, without Hindi — [QwenLM GitHub](https://github.com/QwenLM/Qwen3-TTS)

### Inferences
- Cost per video:
  - A 1,500-word English script is about 9,000 characters. ElevenLabs Starter's 30k credits covers about 3 long videos/month, or roughly 1 long video plus Shorts per week if Shorts are short. Creator ($22) comfortably covers 4 long videos plus 15–20 Shorts.
  - Hindi at ₹3/1k chars: 9,000 chars ≈ ₹27 per long video, so under ₹200/month for the full weekly slate.
- Kokoro runs acceptably on CPU, which matters with no GPU (practitioner knowledge, not verified in this session). Chatterbox and Indic Parler run on free Colab GPUs or Hugging Face Spaces.
- Hindi quality ranking (no benchmark retrieved): Sarvam Bulbul and ElevenLabs are generally regarded as the most natural. Indic Parler-TTS is good but less expressive.

### Gaps
- No independent Hindi TTS quality benchmark (MOS scores) was retrieved.
- ElevenLabs official pricing page was not fetched directly. Figures come from secondary pages dated 2026.

## B4. Visual pipeline with no local GPU; editing, music, thumbnails, upload

### Takeaway
With no GPU, the practical setup is Wan2GP on rented cloud GPUs (RunPod RTX 4090 from ~$0.34/hr community, $0.69/hr secure). Use AI video only for key hero shots. Fill most runtime with AI stills animated with Ken Burns pans, plus free stock footage and motion graphics. Get character consistency by generating one reference image and driving image-to-video from it, or training a LoRA. Edit in DaVinci Resolve (free). CapCut remains banned in India.

### Cited Findings
- RunPod RTX 4090 (24GB): $0.34/hr community/spot, $0.69/hr secure on-demand, billed per second (data as of July 2026) — [deploybase](https://deploybase.ai/articles/rtx-4090-runpod); [RunPod pricing](https://www.runpod.io/gpu-cloud/pricing)
- No dedicated official "Wan2GP RunPod template" was found in search — [search result summary; RunPod templates guide](https://dev.to/vishva_ram/the-complete-guide-to-runpod-templates-cuda-pytorch-environments-for-every-ai-project-4i94)
- MiniMax H3's smallest working setup is ~42.5 GB of weights. ComfyUI says 12 GB VRAM plus offloading can run it — [atlascloud](https://www.atlascloud.ai/blog/guides/minimax-h3-open-source-weights)
- CapCut has been banned in India since June 29, 2020. It is not on Indian app stores, its website is geo-blocked, and leftover installs lose cloud/AI features — [Kripesh Adwani](https://kripeshadwani.com/how-to-use-capcut-in-india/); [fluxnote June 2026](https://fluxnote.io/guides/capcut-questions-answered)
- The Hunyuan license requires conspicuous machine-generated labelling — [Hunyuan LICENSE](https://huggingface.co/tencent/HunyuanVideo-1.5/blob/main/LICENSE). YouTube auto-labels undisclosed photorealistic AI (reported May 2026) — [AIR](https://air.io/en/monetization/youtube-monetization-policy-changes-2026-a-complete-dated-timeline)

### Inferences (practitioner norms; not individually sourced in this session)

**Visual mix for an 8–12 minute video**
- About 10–20% AI video: 15–30 Wan 2.2 / LTX-2 clips of 5s each.
- About 50–60% AI stills (Z-Image / Qwen-Image / FLUX schnell) with Ken Burns pans, done in Resolve.
- The rest: free stock (Pexels/Pixabay), text/motion graphics (Resolve Fusion templates, Canva) and maps/charts.

**Character consistency**
- Create a "character sheet" reference image.
- Use Wan 2.2 image-to-video or reference-to-video models from that sheet.
- Use Qwen-Image-Edit or FLUX Kontext for new poses with the same face.
- Optionally train a character LoRA on a rented GPU, about 1–2 GPU-hours.

**Editing, captions, music, thumbnails**
- DaVinci Resolve (free) for editing and auto-captions. Captions can also come from local Whisper or YouTube auto-captions.
- Music: YouTube Audio Library (free, cleared for YouTube monetization).
- Thumbnails: Canva free or Photopea, with an AI-generated base image.
- Alternatives in India instead of CapCut: Resolve, Shotcut/Kdenlive, and the YouTube Create app (Android).

**Cloud GPU budget**
- Assumption: a 5s 480–720p Wan 2.2 clip takes very roughly 2–8 min on a 4090, depending on the distilled/lightning LoRA, steps and resolution. That works out to about 20–30 clips per week ≈ 2–4 GPU-hours ≈ $1–3 per week on community 4090s.
- Add storage/idle overhead. Realistic total: **~$5–15 per month** of GPU.
- Free alternatives: Google Colab free tier (T4, sessions limited) and Hugging Face Spaces (queues, quotas). Unreliable for weekly production.

**Weekly budget (1 long video, 8–12 min, plus 3–5 Shorts)**

| Stage | Free route | Budget paid route | Time/week (estimate) |
|---|---|---|---|
| Idea/keyword research | YT autocomplete, Trends, Studio Research tab | vidIQ/TubeBuddy basic (price unverified) | 1–2 h |
| Script + fact-check | Free LLM tier | LLM subscription ~$20/mo (unverified this session) | 3–5 h |
| Voice | Kokoro / Chatterbox / Indic Parler (Colab) | ElevenLabs Starter $5–6 or Creator $22/mo; Sarvam ₹3/1k chars | 0.5–1.5 h |
| Images | Z-Image/Qwen-Image/FLUX schnell on rented GPU, or free web tiers | — | 1–2 h |
| AI video | Wan2GP on RunPod 4090 ($0.34/hr) | ~$5–15/mo GPU | 2–4 h (mostly waiting/curating) |
| Edit + captions + music | DaVinci Resolve, Whisper, YT Audio Library | — | 4–6 h |
| Shorts (3–5, cut from the long video) | Resolve vertical timeline | — | 1.5–3 h |
| Thumbnail + title/SEO + upload | Canva free/Photopea; YT Studio | Canva Pro (price unverified) | 1–1.5 h |
| **Total** | **≈$5–15/mo (GPU only)** | **≈$30–60/mo** | **≈14–25 h/week** |

### Gaps
- No primary benchmark was fetched for Wan2GP render time per clip on a 4090 or for a cloud GPU. The time and cost per clip above are estimates.
- Canva Pro, vidIQ and TubeBuddy India pricing were not retrieved.
- YouTube Audio Library licensing terms and Resolve free-version feature limits were not re-verified in this session, though both are long-standing.
- I did not verify whether the Wan2GP Colab/Pinokio cloud options or a community RunPod/Vast.ai template are currently maintained.
