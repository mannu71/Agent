// Gold chariot wheel with a saffron question mark at the hub.
function wheel(cx, cy, r, id) {
  const g = [];
  g.push(`<defs>
    <radialGradient id="${id}glow" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#E8891C" stop-opacity="0.55"/>
      <stop offset="45%" stop-color="#E8891C" stop-opacity="0.18"/>
      <stop offset="100%" stop-color="#E8891C" stop-opacity="0"/>
    </radialGradient>
    <linearGradient id="${id}gold" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0%" stop-color="#F6DE8D"/>
      <stop offset="45%" stop-color="#D4AF37"/>
      <stop offset="100%" stop-color="#8E6B1E"/>
    </linearGradient>
  </defs>`);
  g.push(`<circle cx="${cx}" cy="${cy}" r="${r*1.55}" fill="url(#${id}glow)"/>`);
  const G = `url(#${id}gold)`;
  // sun rays behind the rim
  for (let i = 0; i < 24; i++) {
    const a = (i * 15) * Math.PI / 180, w = 0.06;
    const r1 = r*1.02, r2 = r*(i % 2 ? 1.16 : 1.26);
    const p = [[r1, a-w],[r2, a],[r1, a+w]].map(([rr, aa]) => `${cx+rr*Math.cos(aa)},${cy+rr*Math.sin(aa)}`).join(' ');
    g.push(`<polygon points="${p}" fill="${G}" opacity="0.9"/>`);
  }
  // rim
  g.push(`<circle cx="${cx}" cy="${cy}" r="${r}" fill="none" stroke="${G}" stroke-width="${r*0.11}"/>`);
  g.push(`<circle cx="${cx}" cy="${cy}" r="${r*0.86}" fill="none" stroke="${G}" stroke-width="${r*0.025}"/>`);
  // rim studs
  for (let i = 0; i < 32; i++) {
    const a = i * Math.PI * 2 / 32;
    g.push(`<circle cx="${cx+r*Math.cos(a)}" cy="${cy+r*Math.sin(a)}" r="${r*0.022}" fill="#1B1F3B" opacity="0.6"/>`);
  }
  // 8 major spokes + 8 minor spokes (Konark style)
  for (let i = 0; i < 16; i++) {
    const a = i * Math.PI / 8, major = i % 2 === 0;
    const r0 = r*0.42, r1 = r*0.86, hw = major ? r*0.055 : r*0.025;
    const nx = -Math.sin(a), ny = Math.cos(a);
    const x0 = cx + r0*Math.cos(a), y0 = cy + r0*Math.sin(a);
    const x1 = cx + r1*Math.cos(a), y1 = cy + r1*Math.sin(a);
    g.push(`<polygon points="${x0+nx*hw},${y0+ny*hw} ${x1+nx*hw*0.6},${y1+ny*hw*0.6} ${x1-nx*hw*0.6},${y1-ny*hw*0.6} ${x0-nx*hw},${y0-ny*hw}" fill="${G}"/>`);
    if (major) {
      const xm = cx + r*0.62*Math.cos(a), ym = cy + r*0.62*Math.sin(a);
      g.push(`<circle cx="${xm}" cy="${ym}" r="${r*0.07}" fill="${G}"/><circle cx="${xm}" cy="${ym}" r="${r*0.035}" fill="#1B1F3B"/>`);
    }
  }
  // hub
  g.push(`<circle cx="${cx}" cy="${cy}" r="${r*0.46}" fill="${G}"/>`);
  g.push(`<circle cx="${cx}" cy="${cy}" r="${r*0.40}" fill="#1B1F3B"/>`);
  g.push(`<text x="${cx}" y="${cy + r*0.27}" text-anchor="middle" font-family="Cinzel" font-weight="900" font-size="${r*0.80}" fill="#E8891C">?</text>`);
  return g.join('\n');
}
function stars(w, h, n, seed, maxY) {
  let s = seed; const rnd = () => (s = (s * 16807) % 2147483647) / 2147483647;
  const out = [];
  for (let i = 0; i < n; i++) {
    const y = rnd() * (maxY ?? h);
    out.push(`<circle cx="${rnd()*w}" cy="${y}" r="${0.6 + rnd()*1.8}" fill="#F2E8D5" opacity="${0.15 + rnd()*0.5}"/>`);
  }
  return out.join('');
}
