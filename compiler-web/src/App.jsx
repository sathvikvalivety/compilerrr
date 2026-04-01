import React, { useState, useEffect, useRef } from 'react';
import './App.css';

function tokenColor(typeNum) {
  if (typeNum === '0') return 'val';
  if (typeNum === '2' || typeNum === '4') return 'op';
  if (typeNum === '6' || typeNum === '7') return 'err';
  if (typeNum === '11') return 'kw';
  if (typeNum === '13' || typeNum === '14') return 'num';
  return 'text';
}

function tokenName(typeNum) {
  const names = { '0': 'ID', '2': 'ARITH(+)', '4': 'ARITH(*)', '6': 'RELOP(>)', '7': 'RELOP(<)', '11': 'LOGICAL(&&)', '13': 'LPAREN', '14': 'RPAREN' };
  return names[typeNum] || `TOK_${typeNum}`;
}

function formatTokens(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    const match = line.match(/Token:\s*(\d+),\s*Lexeme:\s*(.*)/);
    if (!match) {
      if (line.includes('Error:')) return <div key={i} className="err" style={{ marginTop: '8px', padding: '8px', background: 'rgba(220, 38, 38, 0.1)', borderLeft: '4px solid var(--red)' }}>{line}</div>;
      return <div key={i}>{line}</div>;
    }
    const type = match[1];
    const lex = match[2];
    return (
      <div key={i}>
        <span className="muted">Token: <span className={tokenColor(type)}>{type}</span> </span>
        <span className="muted">Lexeme: </span><span className={tokenColor(type)}>{lex}</span>
        <span className="comment" style={{ fontSize: '10px', marginLeft: '6px' }}>{tokenName(type)}</span>
      </div>
    );
  });
}

function formatAstRules(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    if (line.includes('AST constructed successfully')) return <div key={i} className="ok" style={{ marginTop: '4px' }}>{line}</div>;
    if (line.includes('Parsing rule:')) return <div key={i}><span className="comment">Parsing rule: </span><span className="val">{line.replace('Parsing rule: ', '')}</span></div>;
    if (line.includes('Error:')) return <div key={i} className="err" style={{ marginTop: '8px', padding: '8px', background: 'rgba(220, 38, 38, 0.1)', borderLeft: '4px solid var(--red)' }}>{line}</div>;
    return <div key={i}>{line}</div>;
  });
}

function ASTNode({ node }) {
  if (!node) return null;
  
  let colorClass = 'val';
  if (node.symbol === '&&' || node.symbol === '||') colorClass = 'kw';
  else if (node.symbol === '>' || node.symbol === '<' || node.symbol === '==' || node.symbol === '!=') colorClass = 'err'; 
  else if (node.symbol === '+' || node.symbol === '*' || node.symbol === '-' || node.symbol === '/') colorClass = 'op';
  
  return (
    <li>
      <a href="#" onClick={e => e.preventDefault()}>
         <span className={colorClass} style={{ fontWeight: 700, fontSize: '14px' }}>{node.symbol}</span>
      </a>
      {node.children && node.children.length > 0 && (
        <ul>
          {node.children.map((child, i) => (
            <ASTNode key={i} node={child} />
          ))}
        </ul>
      )}
    </li>
  );
}

function formatAstTree(text) {
  if (!text.trim()) return null;
  const lines = text.split('\n').filter(Boolean);
  const nodes = [];
  
  for (let line of lines) {
    if (line.includes('AST constructed')) continue;
    if (line.trim() === '') continue;
    
    // Check if error message is captured by mistake
    if (line.includes('Error:')) return <div className="err" style={{ marginTop: '8px', padding: '8px', background: 'rgba(220, 38, 38, 0.1)', borderLeft: '4px solid var(--red)' }}>{line}</div>;

    const match = line.match(/^([| \-\\]*)(.*)/);
    if (!match) continue;
    
    let prefix = match[1];
    let symbol = match[2].trim();
    if (!symbol || symbol === '|') continue; 
    
    let depth = Math.floor(prefix.length / 4);
    nodes.push({ symbol, depth });
  }
  
  if (nodes.length === 0) return null;
  
  const root = { symbol: nodes[0].symbol, children: [] };
  const stack = [{ node: root, depth: 0 }];
  
  for (let i = 1; i < nodes.length; i++) {
    const { symbol, depth } = nodes[i];
    const newNode = { symbol, children: [] };
    
    while (stack.length > 0 && stack[stack.length - 1].depth >= depth) {
      stack.pop();
    }
    
    if (stack.length > 0) {
      stack[stack.length - 1].node.children.push(newNode);
    }
    stack.push({ node: newNode, depth });
  }
  
  return (
    <div className="tf-tree">
      <ul>
        <ASTNode node={root} />
      </ul>
    </div>
  );
}

function formatSemantic(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    if (line === 'Symbol Table:') return <div key={i} className="sec-header" style={{ marginTop: '4px' }}>Symbol Table:</div>;
    if (line.includes('-> type:')) {
      const [id, t] = line.split('-> type:');
      return <div key={i}>&nbsp;&nbsp;<span className="cyan-text">{id.trim()}</span> <span className="muted">-&gt;</span> <span className="kw">type:</span> <span className="val">{t.trim()}</span></div>;
    }
    if (line.includes('[OK]')) return <div key={i}><span className="ok">&#10003;</span> <span className="muted">{line}</span></div>;
    if (line.includes('passed')) return <div key={i} className="ok" style={{ marginTop: '4px' }}>{line}</div>;
    if (line.includes('Error:')) return <div key={i} className="err" style={{ marginTop: '8px', padding: '8px', background: 'rgba(220, 38, 38, 0.1)', borderLeft: '4px solid var(--red)' }}>{line}</div>;
    return <div key={i}>{line}</div>;
  });
}

function formatTac(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    if (line.startsWith('L') && line.includes(':')) return <div key={i} className="label-line">{line}</div>;
    if (line.startsWith('if')) return <div key={i}><span className="kw">if</span> <span className="val">{line.slice(3)}</span></div>;
    if (line.startsWith('goto')) return <div key={i}><span className="kw">goto</span> <span className="label-line">{line.slice(5)}</span></div>;
    const parts = line.split('=');
    if (parts.length === 2 && !line.includes('goto')) {
      return <div key={i}><span className="cyan-text">{parts[0].trim()}</span> <span className="muted">=</span> <span className="val">{parts[1].trim()}</span></div>;
    }
    return <div key={i}>{line}</div>;
  });
}

function formatOptim(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    if (line.includes('[None') || line.includes('Removed')) return <div key={i} className="comment">{line}</div>;
    if (line === 'Optimized TAC Stream:') return <div key={i} style={{ marginTop: '6px' }} className="sec-header">Optimized TAC Stream:</div>;
    if (line.includes('=') || line.includes('goto') || line.startsWith('L')) {
      // re-use tac formatting format logic here
      if (line.startsWith('L') && line.includes(':')) return <div key={i} className="label-line">{line}</div>;
      if (line.startsWith('if')) return <div key={i}><span className="kw">if</span> <span className="val">{line.slice(3)}</span></div>;
      if (line.startsWith('goto')) return <div key={i}><span className="kw">goto</span> <span className="label-line">{line.slice(5)}</span></div>;
      const parts = line.split('=');
      if (parts.length === 2 && !line.includes('goto')) {
        return <div key={i}><span className="cyan-text">{parts[0].trim()}</span> <span className="muted">=</span> <span className="val">{parts[1].trim()}</span></div>;
      }
    }
    return <div key={i} className="muted">{line}</div>;
  });
}

function formatAsm(text) {
  return text.trim().split('\n').filter(Boolean).map((line, i) => {
    if (line.startsWith('L') && line.includes(':')) return <div key={i} className="label-line">{line}</div>;
    if (line.startsWith('JMP') || line.startsWith('JG') || line.startsWith('JL')) {
      const p = line.split(' ');
      return <div key={i}><span className="asm-op">{p[0]}</span> <span className="label-line">{p.slice(1).join(' ')}</span></div>;
    }
    if (line.trim().length > 0) {
      const p = line.split(' ');
      return <div key={i}><span className="asm-op">{p[0]}</span> <span className="asm-reg">{p.slice(1).join(' ')}</span></div>;
    }
    return null;
  });
}

export default function App() {
  const [expr, setExpr] = useState('((a+b)*c>d+e)&&(f>g)');
  const [activeTab, setActiveTab] = useState('tokens');
  const [rawOutput, setRawOutput] = useState('');
  const [status, setStatus] = useState('Ready');
  const [history, setHistory] = useState(['((a+b)*c>d+e)&&(f>g)', 'a+b>c']);
  
  // Phase states
  const [phasesData, setPhasesData] = useState({
    tokens: '', syntax: '', ast: '', semantic: '', tac: '', optim: '', asm: ''
  });

  const evtSourceRef = useRef(null);

  useEffect(() => {
    // Parse raw output into distinct blocks
    const chunks = rawOutput.split(/--- (.+?) ---/);
    let newPhases = { tokens: '', syntax: '', ast: '', semantic: '', tac: '', optim: '', asm: '' };
    
    for (let i = 1; i < chunks.length; i += 2) {
      const title = chunks[i].trim();
      const content = chunks[i+1] ? chunks[i+1].trim() : '';
      
      if (title.includes('Phase 1')) newPhases.tokens = content;
      else if (title.includes('Phase 2')) newPhases.syntax = content;
      else if (title === 'AST') newPhases.ast = content;
      else if (title.includes('Phase 3')) newPhases.semantic = content;
      else if (title.includes('Phase 4')) newPhases.tac = content;
      else if (title.includes('Phase 5')) newPhases.optim = content;
      else if (title.includes('Final x86')) newPhases.asm = content;
    }
    
    // Catch-all: If there's raw output before the first phase (e.g. Usage errs), dump it in the Tokens tab so it's globally visible.
    if (chunks[0] && chunks[0].trim().length > 0 && !chunks[0].includes('Compiling source')) {
       // if it's just 'Compiling source:', skip. If real error, show it.
       const prePhaseText = chunks[0].replace(/Compiling source:.*\n/, '').trim();
       if (prePhaseText) newPhases.tokens = prePhaseText + '\n\n' + newPhases.tokens;
    }
    
    setPhasesData(newPhases);
  }, [rawOutput]);

  const compile = () => {
    if (evtSourceRef.current) {
      evtSourceRef.current.close();
    }
    setRawOutput('');
    setStatus('Compiling...');
    
    // Add to history if not exists
    if (!history.includes(expr)) {
      setHistory(prev => [expr, ...prev].slice(0, 5));
    }

    const url = `/api/compile?expr=${encodeURIComponent(expr)}`;
    const eventSource = new EventSource(url);
    evtSourceRef.current = eventSource;

    eventSource.onmessage = (e) => {
      setRawOutput(prev => prev + e.data + '\n');
      if (e.data.includes('Error:')) {
        setStatus('Error');
      }
    };

    eventSource.addEventListener('done', () => {
      setStatus(prev => prev === 'Error' ? 'Error' : 'Done');
      eventSource.close();
    });

    eventSource.onerror = () => {
      setStatus('Error');
      eventSource.close();
    };
  };

  return (
    <div className="studio">
      <div className="topbar">
        <span className="title">COMPILER STUDIO</span>
        <span className="sep">|</span>
        <span className="stat">6 Phases</span>
        <span className="sep">|</span>
        <span className="stat">Expr: <span id="expr-display">{expr}</span></span>
        <span className="sep">|</span>
        <span className="stat">Mode: <span>AUTO</span></span>
        <span className="sep">|</span>
        <span className="stat">Status: <span id="status-txt" style={{ color: status === 'Done' || status === 'Ready' ? 'var(--green)' : status === 'Error' ? 'var(--red)' : 'var(--yellow)' }}>{status}</span></span>
      </div>

      <div className="input-bar">
        <label>&gt;&gt;</label>
        <input 
          type="text" 
          value={expr} 
          onChange={(e) => setExpr(e.target.value)}
          onKeyDown={(e) => e.key === 'Enter' && compile()} 
          placeholder="Enter expression..." 
          list="historyList"
        />
        <datalist id="historyList">
          {history.map((h, i) => <option key={i} value={h} />)}
        </datalist>
        <button onClick={compile}>Compile</button>
      </div>

      <div className="main-grid">
        <div className="panel" style={{ gridRow: '1 / 3' }}>
          <div className="panel-title">— Editor —</div>
          <div className="tab-bar">
            {['tokens', 'ast', 'tac', 'asm', 'semantic', 'optim'].map(t => (
              <div 
                key={t}
                className={`tab ${activeTab === t ? 'active' : ''}`} 
                onClick={() => setActiveTab(t)}
              >
                {t === 'asm' ? 'x86 Assembly' : t.charAt(0).toUpperCase() + t.slice(1)}
              </div>
            ))}
          </div>

          <div className="panel-body">
            {activeTab === 'tokens' && (
              <div>
                <div className="sec-header">Phase 1: Lexical Analysis</div>
                {formatTokens(phasesData.tokens)}
              </div>
            )}
            
            {activeTab === 'ast' && (
              <div>
                <div className="sec-header">Phase 2: Syntax Analysis — AST</div>
                <div style={{ marginBottom: '8px' }}>
                  {formatAstRules(phasesData.syntax)}
                </div>
                <div>
                  {formatAstTree(phasesData.ast)}
                </div>
              </div>
            )}
            
            {activeTab === 'tac' && (
              <div>
                <div className="sec-header">Phase 4: Intermediate Code (TAC)</div>
                {formatTac(phasesData.tac)}
              </div>
            )}
            
            {activeTab === 'asm' && (
              <div>
                <div className="sec-header">Phase 6: Final x86 Pseudo-Assembly</div>
                {formatAsm(phasesData.asm)}
              </div>
            )}
            
            {activeTab === 'semantic' && (
              <div>
                <div className="sec-header">Phase 3: Semantic Analysis</div>
                {formatSemantic(phasesData.semantic)}
              </div>
            )}
            
            {activeTab === 'optim' && (
              <div>
                <div className="sec-header">Phase 5: Code Optimization</div>
                {formatOptim(phasesData.optim)}
              </div>
            )}
          </div>
        </div>

        <div className="panel phases-panel" style={{ gridRow: '1' }}>
          <div className="panel-title">— Phases —</div>
          <div className="panel-body" style={{ padding: 0 }}>
            <table className="phase-table">
              <thead><tr><th>Phase</th><th>Output</th></tr></thead>
              <tbody>
                {[
                  { name: '1. Lexical', active: !!phasesData.tokens, msg: 'Tokens generated', error: phasesData.tokens.includes('Error:') },
                  { name: '2. Syntax', active: !!phasesData.syntax, msg: 'Valid expression', error: phasesData.syntax.includes('Error:') },
                  { name: '3. Semantic', active: !!phasesData.semantic, msg: 'Checks passed', error: phasesData.semantic.includes('Error:') },
                  { name: '4. Intermediate', active: !!phasesData.tac, msg: 'TAC generated', error: phasesData.tac.includes('Error:') },
                  { name: '5. Optimization', active: !!phasesData.optim, msg: 'Code optimized', error: phasesData.optim.includes('Error:') },
                  { name: '6. Codegen', active: !!phasesData.asm, msg: 'Assembly generated', error: phasesData.asm.includes('Error:') },
                ].map((p, i) => (
                  <tr key={i}>
                    <td className="phase-tag" style={{ color: p.error ? 'var(--red)' : p.active ? 'var(--text)' : 'var(--muted)' }}>{p.name}</td>
                    <td className="phase-ok">
                      {p.error ? <span className="err">&#10007; Failed</span> : p.active ? <span className="ok">&#10003; {p.msg}</span> : <span className="muted">Waiting...</span>}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>

        <div className="panel machine-panel" style={{ gridRow: '2' }}>
          <div className="panel-title">— x86 Assembly Output —</div>
          <div className="panel-body">
            {formatAsm(phasesData.asm)}
          </div>
        </div>
      </div>

      <div className="statusbar">
        {status === 'Ready' ? 'Compiler Studio ready — enter an expression and press Compile' :
         status === 'Compiling...' ? 'Running compiler.exe in real-time...' :
         status === 'Error' ? `Compilation failed for: ${expr}` :
         `Compilation finished for: ${expr}`}
      </div>
    </div>
  );
}
