const ctx = document.getElementById('waterLevelChart').getContext('2d');
let waterLevelChart;
const spotlight = document.getElementById('spotlight');

function plotData(data, label) {
    if (waterLevelChart) {
        waterLevelChart.destroy();
    }

    waterLevelChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: data.map((_, index) => `Day ${index + 1}`),
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
                        text: 'Water Level (m)'
                    }
                }
            }
        }
    });
}

function plotToday() {
    const todayData = [1, 2, 1.5, 1.7, 2.1, 1.8, 1.6]; // Example data
    plotData(todayData, 'Water Level Today');
}

function plotLast7Days() {
    const last7DaysData = [1.5, 1.7, 1.6, 1.8, 2.0, 1.9, 2.2]; // Example data 
    plotData(last7DaysData, 'Water Level Over Last 7 Days');
}

function plotMax() {
    const maxData = [1.2, 1.3, 1.4, 1.5, 1.8, 1.9, 2.0]; // Example max data
    plotData(maxData, 'Maximum Water Level');
}

// Default graph is today graph
plotToday();

// Spotlight script
document.addEventListener('mousemove', function (e) {
    spotlight.style.left = `${e.clientX}px`;
    spotlight.style.top = `${e.clientY}px`;
});
