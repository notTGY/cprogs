const { spawn } = require('child_process');

const mpiProgram = './1.out 1000000000';

function executeMpirun(processes) {
  return new Promise((resolve, reject) => {
    const startTime = Date.now();
    const command = `mpiexec -n ${processes} --use-hwthread-cpus --oversubscribe ${mpiProgram}`;
    const child = spawn(command, { shell: true });

    child.stdout.pipe(process.stdout);
    child.stderr.pipe(process.stderr);

    child.on('close', (code) => {
      const endTime = Date.now();
      const executionTime = endTime - startTime; // Time in milliseconds
      resolve(executionTime);
    });

    child.on('error', (err) => {
      console.error(`Error executing mpirun: ${err}`);
      reject(err);
    });
  });
}

async function main() {
  const times = [];
  for (let processes = 1; processes <= 28; processes++) {
    const time = await executeMpirun(processes);
    times.push(time);
  }
  console.log(times.join(','));
}

main().catch((err) => {
  console.error('Script failed:', err);
});
