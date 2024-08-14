const ctx = document.getElementById('waterLevelChart').getContext('2d');
let waterLevelChart;
const spotlight = document.getElementById('spotlight');

function plotData(data, label, xLabels) {
    if (waterLevelChart) {
        waterLevelChart.destroy();
    }

    waterLevelChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: xLabels,
            datasets: [{
                label: label,
                data: data,
                fill: false,
                borderColor: 'rgb(75, 192, 192)',
                tension: 0.1
            }]
        },
        options: {
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Time'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Water Level (cm)'
                    },
                    min: 0,
                    max: 350
                }
            }
        }
    });
}

function plotToday() {
    const todayData = Array.from({length: 24}, () => Math.floor(Math.random() * 350)); // example data
    const xLabels = Array.from({length: 24}, (_, i) => `${i}:00`);
    plotData(todayData, 'Water Level Today', xLabels);
}

function plotLast7Days() {
    const last7DaysData = Array.from({length: 14}, () => Math.floor(Math.random() * 350)); // example data
    const xLabels = Array.from({length: 14}, (_, i) => `Day ${Math.floor(i / 2) + 1} - ${i % 2 === 0 ? 'AM' : 'PM'}`);
    plotData(last7DaysData, 'Water Level Over Last 7 Days', xLabels);
}

function plotMax() {
    const maxData = Array.from({length: 30}, () => Math.floor(Math.random() * 350)); // example data
    const xLabels = Array.from({length: 30}, (_, i) => `Day ${i + 1}`);
    plotData(maxData, 'Water Level Over Last 30 Days', xLabels);
}

plotToday();

document.addEventListener('mousemove', function (e) {
    spotlight.style.left = `${e.clientX}px`;
    spotlight.style.top = `${e.clientY}px`;

    const bgShiftX = e.clientX * 0.05;
    const bgShiftY = e.clientY * 0.05;

    document.body.style.backgroundPosition = `${bgShiftX}px ${bgShiftY}px`;
});

document.addEventListener('mouseleave', function () {
    document.body.style.backgroundPosition = 'top left';
});
