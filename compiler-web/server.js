import express from 'express';
import { spawn } from 'child_process';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const port = 3001;

app.get('/api/compile', (req, res) => {
  const { expr } = req.query;

  // Input sanitization: strip dangerous characters, but allow # for valid invalid char tests
  const safeExpr = expr ? String(expr).replace(/[^a-zA-Z0-9+\-*/><!&|()=# ]/g, '') : '';

  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');
  res.flushHeaders();

  // Path to the executable (moves up from compiler-web folder)
  const compilerPath = path.join(__dirname, '..', 'compiler.exe');
  
  const child = spawn(compilerPath, [safeExpr]);

  child.stdout.on('data', (data) => {
    // Send standard output lines to the client
    const lines = data.toString('utf-8').split('\n');
    for (const line of lines) {
      if (line !== undefined) {
        // SSE format: data: <message>\n\n
        res.write(`data: ${line}\n\n`);
      }
    }
  });

  child.stderr.on('data', (data) => {
    // Send standard error lines to the client
    const lines = data.toString('utf-8').split('\n');
    for (const line of lines) {
      if (line !== undefined) {
        res.write(`data: ${line}\n\n`);
      }
    }
  });

  child.on('close', (code) => {
    res.write(`data: --- Process Exited With Code ${code} ---\n\n`);
    res.write('event: done\ndata: {}\n\n');
    res.end();
  });

  child.on('error', (err) => {
    res.write(`data: Server Error: Failed to start compiler.exe. ${err.message}\n\n`);
    res.write('event: done\ndata: {}\n\n');
    res.end();
  });
});

app.listen(port, () => {
  console.log(`Backend listening at http://localhost:${port}`);
});
