/* Concept Desk front end: live chart, strategy signals, paper order ticket, account views. */
(function () {
  "use strict";
  const $ = (id) => document.getElementById(id);
  const esc = (s) => String(s ?? "").replace(/[&<>"]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
  const CUR = { crypto: "$", us: "$", india: "₹" };
  const STEP = { crypto: 0.0001, us: 1, india: 1 };
  const TFS = ["1m", "5m", "15m", "1h", "4h", "1D"];
  const TF_MIN = { "1m": 1, "5m": 5, "15m": 15, "1h": 60, "4h": 240, "1D": 1440 };

  const S = {
    markets: {}, watchlists: {}, quotes: {}, signals: {}, status: {}, accounts: {}, positions: [], orders: [], trades: [],
    market: "crypto", symbol: null, tf: "15m", sel: null, side: "buy", tab: "positions", activeOnly: true, risk: 0.5,
    chartKey: null, bars: [], journal: 0,
  };
  try { Object.assign(S, JSON.parse(localStorage.getItem("desk-ui") || "{}")); } catch (e) {}
  const remember = () => { try { localStorage.setItem("desk-ui", JSON.stringify({ market: S.market, symbol: S.symbol, tf: S.tf, tab: S.tab, activeOnly: S.activeOnly })); } catch (e) {} };

  // ------------------------------------------------------------ formatting
  function dec(p) { p = Math.abs(p || 0); return p >= 1000 ? 2 : p >= 10 ? 2 : p >= 1 ? 4 : 5; }
  function px(v, ref) { if (v == null || isNaN(v)) return "–"; const d = dec(ref ?? v); return Number(v).toLocaleString("en-US", { minimumFractionDigits: d, maximumFractionDigits: d }); }
  function money(v, m) { if (v == null || isNaN(v)) return "–"; const s = v < 0 ? "−" : ""; return s + CUR[m] + Math.abs(v).toLocaleString(m === "india" ? "en-IN" : "en-US", { maximumFractionDigits: 2, minimumFractionDigits: 2 }); }
  function pct(v) { return v == null ? "" : (v >= 0 ? "+" : "") + v.toFixed(2) + "%"; }
  function rr(v) { return v == null ? "–" : (v > 0 ? "+" : "") + Number(v).toFixed(2) + "R"; }
  function cls(v) { return v == null ? "" : v > 0 ? "up" : v < 0 ? "down" : ""; }
  function qtyFmt(q, m) { return m === "crypto" ? Number(q).toFixed(4) : String(Math.round(q)); }
  function when(sec) { return new Date(sec * 1000).toLocaleString([], { month: "short", day: "numeric", hour: "2-digit", minute: "2-digit" }); }
  function localTime(utcStr) { return when(Date.parse(utcStr.replace(" ", "T") + ":00Z") / 1000); }
  const key = (m, s) => m + "|" + s;

  function toast(msg, err) {
    const el = document.createElement("div");
    el.className = "toast" + (err ? " err" : "");
    el.textContent = msg;
    $("toasts").appendChild(el);
    setTimeout(() => el.remove(), err ? 7000 : 3500);
  }

  // ------------------------------------------------------------ socket
  let ws, backoff = 1;
  function connect() {
    ws = new WebSocket((location.protocol === "https:" ? "wss://" : "ws://") + location.host + "/ws");
    ws.onopen = () => { backoff = 1; if (S.symbol) requestChart(); };
    ws.onmessage = (e) => handle(JSON.parse(e.data));
    ws.onclose = () => { renderFeeds(true); setTimeout(connect, backoff * 1000); backoff = Math.min(15, backoff * 2); };
  }
  function send(msg) { if (ws && ws.readyState === 1) ws.send(JSON.stringify(msg)); else toast("Not connected to the app server", true); }

  function handle(m) {
    switch (m.type) {
      case "snapshot":
        S.markets = m.markets; S.watchlists = m.watchlists; S.status = m.status; S.risk = m.risk_pct; S.alpaca = m.alpaca;
        m.quotes.forEach((q) => (S.quotes[key(q.market, q.symbol)] = q));
        S.signals = {};
        m.signals.forEach((s) => (S.signals[key(s.market, s.symbol)] ||= []).push(s));
        setAccount(m);
        if (!S.symbol || !(S.watchlists[S.market] || []).includes(S.symbol)) S.symbol = (S.watchlists[S.market] || [])[0];
        renderAll(); requestChart();
        if (!$("o-size").dataset.touched) $("o-size").value = S.risk;
        break;
      case "prices":
        m.prices.forEach((q) => (S.quotes[key(q.market, q.symbol)] = q));
        renderWatch(); renderSymHead(); renderEstimate();
        break;
      case "bar":
        if (m.market === S.market && m.symbol === S.symbol && TF_MIN[S.tf] === m.tf && candle) {
          candle.update(m.bar); volume.update({ time: m.bar.time, value: m.bar.volume, color: volColor(m.bar) });
          const last = S.bars[S.bars.length - 1];
          if (!last || m.bar.time > last.time) S.bars.push(m.bar); else S.bars[S.bars.length - 1] = m.bar;
        }
        break;
      case "chart":
        if (m.market === S.market && m.symbol === S.symbol) {
          S.signals[key(m.market, m.symbol)] = m.signals;
          setChart(m.bars); renderSignals();
        }
        break;
      case "signals":
        S.signals[key(m.market, m.symbol)] = m.signals;
        if (m.market === S.market && m.symbol === S.symbol) { renderSignals(); drawMarkers(); }
        renderWatch();
        break;
      case "account": setAccount(m); renderAccountViews(); drawLines(); break;
      case "status": S.status = m.status; renderFeeds(); break;
      case "notice": toast(m.message); break;
      case "error": toast(m.message, true); break;
    }
  }
  function setAccount(m) { S.accounts = m.accounts; S.positions = m.positions; S.orders = m.orders; S.trades = m.trades; S.journal = m.journal?.lines || 0; }

  // ------------------------------------------------------------ header and watchlist
  function renderFeeds(disconnected) {
    $("feeds").innerHTML = disconnected ? '<span class="feed warn"><span class="dot"></span>Reconnecting to app…</span>' :
      Object.entries(S.status).map(([mk, st]) => {
        const live = /live/.test(st.status), warn = /retry|reconnect|fail|delayed|loading|building/.test(st.status);
        return `<span class="feed ${live ? "live" : warn ? "warn" : ""}" title="Memory: ${esc(st.memory)}"><span class="dot"></span>${esc(st.label)} · ${esc(st.status)}${st.open || /closed/.test(st.status) ? "" : " · closed now"}</span>`;
      }).join("");
  }
  function renderAcctSummary() {
    $("acct-summary").innerHTML = Object.entries(S.accounts).map(([mk, a]) =>
      `<div><span class="k">${esc(S.markets[mk] || mk)} equity</span><span class="v num ${cls(a.equity - a.start_cash)}">${money(a.equity, mk)}</span></div>`).join("");
  }
  function renderMarketTabs() {
    $("market-tabs").innerHTML = Object.entries(S.markets).map(([mk, label]) =>
      `<button role="tab" data-m="${mk}" aria-selected="${mk === S.market}">${esc(label)}</button>`).join("");
  }
  function renderWatch() {
    const syms = S.watchlists[S.market] || [];
    $("watch-list").innerHTML = syms.map((s) => {
      const q = S.quotes[key(S.market, s)] || {};
      const sig = q.signals ? `<span class="badge ${q.trade_ok ? "trade" : ""}">${q.signals} signal${q.signals > 1 ? "s" : ""}</span>` : "";
      return `<div class="wrow" data-s="${esc(s)}" aria-current="${s === S.symbol}" role="button" tabindex="0">
        <span class="s">${esc(s)} ${sig}</span><span class="p num">${px(q.price)}</span>
        <span class="sg">${q.flow === false ? "no order-flow data" : ""}</span><span class="c num ${cls(q.change)}">${pct(q.change)}</span></div>`;
    }).join("") || '<div class="empty">Loading…</div>';
  }
  function renderSymHead() {
    const q = S.quotes[key(S.market, S.symbol)] || {};
    $("sym-name").textContent = S.symbol ? `${S.symbol} · ${S.markets[S.market] || ""}` : "—";
    $("sym-price").textContent = px(q.price);
    const c = $("sym-change"); c.textContent = pct(q.change); c.className = "chg num " + cls(q.change);
  }
  function renderTfTabs() {
    $("tf-tabs").innerHTML = TFS.map((t) => `<button data-tf="${t}" aria-selected="${t === S.tf}">${t}</button>`).join("");
  }

  // ------------------------------------------------------------ chart
  let chart, candle, volume, lines = [];
  function css(v) { return getComputedStyle(document.documentElement).getPropertyValue(v).trim(); }
  function volColor(b) { return b.close >= b.open ? "rgba(31,177,119,0.35)" : "rgba(229,83,75,0.35)"; }
  function initChart() {
    const el = $("chart");
    chart = LightweightCharts.createChart(el, {
      layout: { background: { color: css("--bg") }, textColor: css("--muted") },
      grid: { vertLines: { color: css("--line") }, horzLines: { color: css("--line") } },
      rightPriceScale: { borderColor: css("--line") },
      timeScale: { borderColor: css("--line"), timeVisible: true, secondsVisible: false,
        tickMarkFormatter: (t, type) => {
          const d = new Date(t * 1000);
          if (type < 3) return d.toLocaleDateString([], { month: "short", day: "numeric" });
          return d.getHours() === 0 && d.getMinutes() === 0 ? d.toLocaleDateString([], { month: "short", day: "numeric" })
            : d.toLocaleTimeString([], { hour: "2-digit", minute: "2-digit" });
        } },
      localization: { timeFormatter: (t) => when(t) },
      crosshair: { mode: 0 },
    });
    candle = chart.addCandlestickSeries({ upColor: css("--buy"), downColor: css("--sell"), borderVisible: false,
      wickUpColor: css("--buy"), wickDownColor: css("--sell") });
    volume = chart.addHistogramSeries({ priceScaleId: "", priceFormat: { type: "volume" } });
    volume.priceScale().applyOptions({ scaleMargins: { top: 0.82, bottom: 0 } });
    new ResizeObserver(() => chart.applyOptions({ width: el.clientWidth, height: el.clientHeight })).observe(el);
  }
  function requestChart() {
    if (!S.symbol) return;
    S.chartKey = key(S.market, S.symbol) + "|" + S.tf;
    $("chart-empty").hidden = false; $("chart-empty").textContent = "Loading prices…";
    send({ type: "chart", market: S.market, symbol: S.symbol, tf: S.tf });
  }
  function setChart(bars) {
    S.bars = bars;
    const ref = bars.length ? bars[bars.length - 1].close : 1, d = dec(ref);
    candle.applyOptions({ priceFormat: { type: "price", precision: d, minMove: Math.pow(10, -d) } });
    candle.setData(bars);
    volume.setData(bars.map((b) => ({ time: b.time, value: b.volume, color: volColor(b) })));
    $("chart-empty").hidden = bars.length > 0;
    if (!bars.length) $("chart-empty").textContent = "No price history yet for this market.";
    chart.timeScale().fitContent();
    drawMarkers(); drawLines();
  }
  function barTimeFor(sec) {  // the chart bar that contains time `sec`
    const b = S.bars; let lo = 0, hi = b.length - 1, ans = null;
    while (lo <= hi) { const mid = (lo + hi) >> 1; if (b[mid].time <= sec) { ans = b[mid].time; lo = mid + 1; } else hi = mid - 1; }
    return ans;
  }
  function chartSignals() {
    const all = S.signals[key(S.market, S.symbol)] || [];
    const tf = TF_MIN[S.tf];
    return [15, 60, 240].includes(tf) ? all.filter((s) => s.tf === tf) : all;
  }
  function drawMarkers() {
    if (!candle || !S.bars.length) return;
    const ms = [];
    for (const s of chartSignals()) {
      const t = barTimeFor((s.t - s.tf) * 60);
      if (t == null) continue;
      const long = s.side === "long", live = s.status === "pending" || s.status === "open", sel = S.sel === s.id;
      const color = s.verdict.status === "trade" ? css("--buy") : long ? "#5b8def" : "#e3a33a";
      // Live and selected signals get labelled arrows; finished ones a small dot, to keep the chart readable.
      if (live || sel) ms.push({ time: t, position: long ? "belowBar" : "aboveBar", shape: long ? "arrowUp" : "arrowDown",
        color, text: sel ? "▶ " + s.pattern : "" });
      else ms.push({ time: t, position: long ? "belowBar" : "aboveBar", shape: "circle", color: color + "88", size: 0.4 });
    }
    ms.sort((a, b) => a.time - b.time);
    candle.setMarkers(ms);
  }
  function drawLines() {
    if (!candle) return;
    lines.forEach((l) => candle.removePriceLine(l)); lines = [];
    const add = (price, color, title, style = 2) => price && lines.push(candle.createPriceLine({ price, color, lineWidth: 1, lineStyle: style, axisLabelVisible: true, title }));
    const sel = (S.signals[key(S.market, S.symbol)] || []).find((s) => s.id === S.sel);
    if (sel) { add(sel.entry, css("--accent"), "entry"); add(sel.stop, css("--sell"), "stop"); add(sel.target, css("--buy"), "target"); }
    for (const p of S.positions.filter((p) => p.market === S.market && p.symbol === S.symbol)) {
      add(p.avg, css("--ink"), `position ${p.qty > 0 ? "long" : "short"}`, 0); add(p.stop, css("--sell"), "SL", 0); add(p.target, css("--buy"), "TP", 0);
    }
    for (const o of S.orders.filter((o) => o.market === S.market && o.symbol === S.symbol)) add(o.price, css("--warn"), `${o.type} ${o.side > 0 ? "buy" : "sell"} #${o.id}`, 1);
    $("legend").innerHTML = `<span>▲▼ strategy signals (${esc(TF_MIN[S.tf] >= 15 && TF_MIN[S.tf] <= 240 ? S.tf : "all timeframes")})</span>
      <span>arrows = active signals · dots = finished ones</span>
      <span><i style="background:${css("--buy")}"></i>green = memory says Trade</span>
      <span>Times in your local time zone</span>`;
  }

  // ------------------------------------------------------------ signals
  function verdictText(v, market) {
    if (v.status === "trade") return `Trade · ${rr(v.mean_r)} avg over ${v.cases.toLocaleString()} past setups`;
    if (v.status === "skip") return `Skip · ${rr(v.mean_r)} avg over ${v.cases.toLocaleString()} past setups`;
    return v.cases ? `Untested · only ${v.cases} past setups in ${S.markets[market]}` : `Untested in ${S.markets[market]}`;
  }
  function tradable(s) {
    if (s.status === "pending") return true;
    if (s.status !== "open") return false;
    const q = S.quotes[key(s.market, s.symbol)] || {}, p = q.price, long = s.side === "long";
    return p != null && (long ? p > s.stop && p < s.target : p < s.stop && p > s.target);
  }
  function renderSignals() {
    let list = S.signals[key(S.market, S.symbol)] || [];
    if (S.activeOnly) list = list.filter((s) => s.status === "pending" || s.status === "open");
    $("signal-list").innerHTML = list.length ? list.slice(0, 40).map((s) => {
      const tfL = s.tf === 60 ? "1h" : s.tf === 240 ? "4h" : s.tf + "m";
      const status = { pending: s.order === "limit" ? "limit order waiting" : "entry now (next bar)", open: `running ${rr(s.r)}`, closed: `closed ${rr(s.r)}`, expired: "expired" }[s.status];
      const ok = tradable(s);
      return `<div class="sig ${S.sel === s.id ? "sel" : ""}" data-id="${esc(s.id)}">
        <div class="top-row"><span class="t">${esc(s.name)} <span class="meta">${tfL}</span></span><span class="pill ${s.side}">${s.side === "long" ? "BUY" : "SELL"}</span></div>
        <div class="meta">${localTime(s.time)} · ${esc(status)} · ${esc(s.context.trend || "?")}trend</div>
        <div class="levels num"><div><span class="k">Entry</span>${px(s.entry)}</div><div><span class="k">Stop</span>${px(s.stop, s.entry)}</div><div><span class="k">Target</span>${px(s.target, s.entry)}</div></div>
        <div class="verdict ${s.verdict.status}">Memory: ${esc(verdictText(s.verdict, s.market))}</div>
        <div class="explain">${esc(s.explain)}</div>
        <div class="acts"><button data-act="show">Show on chart</button><button class="go" data-act="trade" ${ok ? "" : "disabled"} title="${ok ? "" : "This signal is no longer tradable"}">${s.status === "open" ? "Paper trade late" : "Paper trade this"}</button></div>
      </div>`;
    }).join("") : `<div class="empty">${S.activeOnly ? "No active signals for this symbol right now." : "No signals in the last 3 days."}<br>Signals appear when a 15m, 1h or 4h bar closes.</div>`;
  }
  function useSignal(s) {
    S.sel = s.id;
    S.side = s.side === "long" ? "buy" : "sell";
    $("o-type").value = s.status === "pending" && s.order === "limit" ? "limit" : "market";
    $("o-price").value = $("o-type").value === "limit" ? px(s.entry).replace(/,/g, "") : "";
    $("o-stop").value = px(s.stop, s.entry).replace(/,/g, "");
    $("o-target").value = px(s.target, s.entry).replace(/,/g, "");
    $("o-sizeby").value = "risk"; $("o-size").value = S.risk;
    $("ticket-src").textContent = `from ${s.name} (${s.tf === 60 ? "1h" : s.tf === 240 ? "4h" : s.tf + "m"})`;
    $("ticket").dataset.signal = s.id;
    renderTicket(); renderSignals(); drawMarkers(); drawLines();
    $("ticket").scrollIntoView({ behavior: "smooth", block: "nearest" });
  }

  // ------------------------------------------------------------ order ticket
  const num = (id) => { const v = parseFloat(String($(id).value).replace(/,/g, "")); return isNaN(v) ? null : v; };
  function renderTicket() {
    $("btn-buy").setAttribute("aria-pressed", S.side === "buy"); $("btn-sell").setAttribute("aria-pressed", S.side === "sell");
    $("o-price").disabled = $("o-type").value === "market";
    if ($("o-type").value === "market") $("o-price").value = "";
    $("o-size-label").firstChild.textContent = $("o-sizeby").value === "risk" ? "Risk % " : "Quantity ";
    const b = $("o-submit");
    b.textContent = `Place paper ${S.side} · ${S.symbol || ""}`;
    b.style.background = S.side === "buy" ? css("--buy") : css("--sell");
    renderEstimate();
  }
  function renderEstimate() {
    if (!S.symbol) return;
    const m = S.market, q = S.quotes[key(m, S.symbol)] || {}, a = S.accounts[m] || {};
    const entry = $("o-type").value === "market" ? q.price : num("o-price");
    const stop = num("o-stop"), target = num("o-target"), size = num("o-size");
    if (!entry) { $("o-est").textContent = "Waiting for a price…"; return; }
    let qty = null;
    if ($("o-sizeby").value === "qty") qty = size;
    else if (stop && size) qty = Math.floor((a.equity * size / 100 / Math.abs(entry - stop)) / STEP[m]) * STEP[m];
    const parts = [];
    if (qty) parts.push(`Qty ${qtyFmt(qty, m)} · notional ${money(qty * entry, m)}`);
    if (qty && stop) parts.push(`Risk ${money(qty * Math.abs(entry - stop), m)}`);
    if (stop && target) parts.push(`Reward:risk ${(Math.abs(target - entry) / Math.abs(entry - stop)).toFixed(2)}`);
    const fee = m === "crypto" ? 0.0007 : m === "india" ? 0.0006 : 0.00005;
    if (qty) parts.push(`fees ≈ ${money(qty * entry * fee, m)} round trip`);
    $("o-est").textContent = parts.join(" · ") || "Set a stop-loss to size by risk.";
  }
  function submitOrder() {
    const type = $("o-type").value;
    const msg = { type: "order", market: S.market, symbol: S.symbol, side: S.side, order_type: type,
      price: type === "market" ? null : num("o-price"), stop: num("o-stop"), target: num("o-target"),
      signal: $("ticket").dataset.signal || null };
    if ($("o-sizeby").value === "qty") msg.qty = num("o-size"); else msg.risk_pct = num("o-size");
    send(msg);
  }

  // ------------------------------------------------------------ bottom panels
  function renderAccountViews() { renderAcctSummary(); renderBottom(); }
  function renderBottom() {
    for (const b of document.querySelectorAll("#bottom-tabs button")) b.setAttribute("aria-selected", b.dataset.tab === S.tab);
    const el = $("bottom-body");
    if (S.tab === "positions") {
      el.innerHTML = S.positions.length ? `<table><thead><tr><th>Market</th><th>Symbol</th><th>Side</th><th class="r">Qty</th><th class="r">Avg</th><th class="r">Last</th><th class="r">P&amp;L</th><th class="r">R</th><th>Stop-loss</th><th>Target</th><th></th></tr></thead><tbody>${
        S.positions.map((p) => `<tr><td>${esc(S.markets[p.market])}</td><td>${esc(p.symbol)}</td><td><span class="pill ${p.qty > 0 ? "long" : "short"}">${p.qty > 0 ? "LONG" : "SHORT"}</span></td>
        <td class="r num">${qtyFmt(Math.abs(p.qty), p.market)}</td><td class="r num">${px(p.avg)}</td><td class="r num">${px(p.last, p.avg)}</td>
        <td class="r num ${cls(p.unrealised)}">${money(p.unrealised, p.market)}</td><td class="r num ${cls(p.r)}">${rr(p.r)}</td>
        <td><input class="num" data-f="stop" value="${p.stop ?? ""}" aria-label="Stop-loss for ${esc(p.symbol)}"></td><td><input class="num" data-f="target" value="${p.target ?? ""}" aria-label="Target for ${esc(p.symbol)}"></td>
        <td><button data-act="save" data-m="${p.market}" data-s="${esc(p.symbol)}">Save</button> <button data-act="close" data-m="${p.market}" data-s="${esc(p.symbol)}">Close</button></td></tr>`).join("")}</tbody></table>` :
        '<div class="empty">No open paper positions. Tap "Paper trade this" on a signal, or use the order ticket.</div>';
    } else if (S.tab === "orders") {
      el.innerHTML = S.orders.length ? `<table><thead><tr><th>#</th><th>Market</th><th>Symbol</th><th>Side</th><th>Type</th><th class="r">Qty</th><th class="r">Price</th><th class="r">Stop</th><th class="r">Target</th><th>Placed</th><th></th></tr></thead><tbody>${
        S.orders.map((o) => `<tr><td>${o.id}</td><td>${esc(S.markets[o.market])}</td><td>${esc(o.symbol)}</td><td><span class="pill ${o.side > 0 ? "long" : "short"}">${o.side > 0 ? "BUY" : "SELL"}</span></td><td>${esc(o.type)}</td>
        <td class="r num">${qtyFmt(o.qty, o.market)}</td><td class="r num">${px(o.price)}</td><td class="r num">${px(o.stop, o.price)}</td><td class="r num">${px(o.target, o.price)}</td><td>${when(o.created)}</td>
        <td><button data-act="cancel" data-id="${o.id}">Cancel</button></td></tr>`).join("")}</tbody></table>` : '<div class="empty">No open orders.</div>';
    } else if (S.tab === "history") {
      el.innerHTML = S.trades.length ? `<table><thead><tr><th>Closed</th><th>Market</th><th>Symbol</th><th>Side</th><th class="r">Qty</th><th class="r">Entry</th><th class="r">Exit</th><th class="r">P&amp;L</th><th class="r">R</th><th>Why</th><th>Signal</th></tr></thead><tbody>${
        S.trades.map((t) => `<tr><td>${when(t.exit_t)}</td><td>${esc(S.markets[t.market])}</td><td>${esc(t.symbol)}</td><td>${t.side > 0 ? "Long" : "Short"}</td><td class="r num">${qtyFmt(t.qty, t.market)}</td>
        <td class="r num">${px(t.entry)}</td><td class="r num">${px(t.exit, t.entry)}</td><td class="r num ${cls(t.pnl)}">${money(t.pnl, t.market)}</td><td class="r num ${cls(t.r)}">${rr(t.r)}</td>
        <td>${esc(t.reason)}</td><td class="muted">${esc((t.signal || "manual").split("|")[3] || "manual")}</td></tr>`).join("")}</tbody></table>` : '<div class="empty">No closed paper trades yet.</div>';
    } else {
      el.innerHTML = `<div class="cards">${Object.entries(S.accounts).map(([mk, a]) => `<div class="card">
        <span class="muted">${esc(S.markets[mk])} paper account</span><span class="big num ${cls(a.equity - a.start_cash)}">${money(a.equity, mk)}</span>
        <span class="small">Started with ${money(a.start_cash, mk)} · realised ${money(a.realized, mk)} · open ${money(a.unrealised, mk)}</span>
        <span class="small muted">Fees and charges paid ${money(a.fees, mk)}</span>
        <div><button class="ghost" data-act="reset" data-m="${mk}">Reset account</button></div></div>`).join("")}</div>
        <p class="small muted">Every order, fill and exit is recorded in a tamper-evident journal (${S.journal} entries) at app/data/journal.log.</p>`;
    }
  }

  // ------------------------------------------------------------ events
  function renderAll() { renderFeeds(); renderAcctSummary(); renderMarketTabs(); renderWatch(); renderSymHead(); renderTfTabs(); renderSignals(); renderTicket(); renderBottom(); }

  document.addEventListener("click", (e) => {
    const t = e.target.closest("button, .wrow, .sig");
    if (!t) return;
    if (t.matches("#market-tabs button")) { S.market = t.dataset.m; S.symbol = (S.watchlists[S.market] || [])[0]; S.sel = null; delete $("ticket").dataset.signal; $("ticket-src").textContent = ""; remember(); renderAll(); requestChart(); }
    else if (t.matches(".wrow")) { S.symbol = t.dataset.s; S.sel = null; delete $("ticket").dataset.signal; $("ticket-src").textContent = ""; remember(); renderWatch(); renderSymHead(); renderSignals(); renderTicket(); requestChart(); }
    else if (t.matches("#tf-tabs button")) { S.tf = t.dataset.tf; remember(); renderTfTabs(); requestChart(); }
    else if (t.matches(".side-btn")) { S.side = t.dataset.side; renderTicket(); }
    else if (t.id === "o-submit") submitOrder();
    else if (t.matches("#bottom-tabs button")) { S.tab = t.dataset.tab; remember(); renderBottom(); }
    else if (t.dataset.act === "show" || t.dataset.act === "trade") {
      const id = t.closest(".sig").dataset.id, s = (S.signals[key(S.market, S.symbol)] || []).find((x) => x.id === id);
      if (!s) return;
      if (t.dataset.act === "trade") useSignal(s);
      else {
        S.sel = id; renderSignals(); drawMarkers(); drawLines();
        const tt = barTimeFor((s.t - s.tf) * 60), span = TF_MIN[S.tf] * 60;
        if (tt) chart.timeScale().setVisibleRange({ from: tt - span * 60, to: tt + span * 30 });
      }
    }
    else if (t.dataset.act === "cancel") send({ type: "cancel", id: t.dataset.id });
    else if (t.dataset.act === "close") send({ type: "close", market: t.dataset.m, symbol: t.dataset.s });
    else if (t.dataset.act === "save") {
      const row = t.closest("tr");
      send({ type: "bracket", market: t.dataset.m, symbol: t.dataset.s, stop: row.querySelector('[data-f="stop"]').value || null, target: row.querySelector('[data-f="target"]').value || null });
    }
    else if (t.dataset.act === "reset") {
      if (t.dataset.armed) send({ type: "reset", market: t.dataset.m });
      else { t.dataset.armed = "1"; t.textContent = "Click again to reset to the starting cash"; setTimeout(() => { delete t.dataset.armed; t.textContent = "Reset account"; }, 4000); }
    }
    else if (t.id === "open-settings") { $("s-risk").value = S.risk; $("settings").showModal(); }
    else if (t.id === "hide-banner") $("banner").hidden = true;
  });
  document.addEventListener("keydown", (e) => { if (e.key === "Enter" && e.target.matches(".wrow")) e.target.click(); });
  ["o-type", "o-sizeby"].forEach((id) => $(id).addEventListener("change", renderTicket));
  ["o-price", "o-stop", "o-target", "o-size"].forEach((id) => $(id).addEventListener("input", () => { if (id === "o-size") $(id).dataset.touched = "1"; renderEstimate(); }));
  $("active-only").checked = S.activeOnly;
  $("active-only").addEventListener("change", (e) => { S.activeOnly = e.target.checked; remember(); renderSignals(); });
  $("s-save").addEventListener("click", () => {
    const msg = { type: "settings", risk_pct: parseFloat($("s-risk").value) || 0.5 };
    if ($("s-key").value.trim()) { msg.alpaca_key = $("s-key").value.trim(); msg.alpaca_secret = $("s-secret").value.trim(); }
    S.risk = msg.risk_pct; send(msg);
  });

  initChart();
  renderAll();
  connect();
})();
