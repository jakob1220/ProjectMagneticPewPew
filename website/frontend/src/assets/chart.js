/**
 * Checks if a value is a float.       
 * @param {number} n - the value to check       
 * @returns {boolean} - true if the value is a float, false otherwise       
 */
const isFloat = (n) => {
  return Number(n) === n && n % 1 !== 0;
}

/**
 * The data for the series of the chart.           
 * @type {Array<{name: string, data: Array<number>}>}           
 */
exports.series = [
  {
    name: "Geschwindigkeit",
    data: [],
  },
  {
    name: "Beschleunigung",
    data: [],
  },
];

exports.chartOptions = {
  /**
   * The chart object for the chart that is displayed on the page.           
   * @type {object}           
   * @property {string} foreColor - The color of the text in the chart.           
   * @property {number} height - The height of the chart.           
   * @property {string} type - The type of chart to display.           
   * @property {object} toolbar - The toolbar object for the chart.           
   * @property {boolean} toolbar.show - Whether or not to show the toolbar.           
   * @property {number} toolbar.offsetX - The x offset of the toolbar.           
   * @property {number} toolbar.offsetY - The y
   */
  chart: { 
    foreColor: 'whitesmoke',
    height: 350,
    type: "area",
    toolbar: {
      show: true,
      offsetX: 0,
      offsetY: 0,
      tools: {
        download: true,
        selection: false,
        zoom: false,
        zoomin: false,
        zoomout: false,
        pan: false,
        reset: false,
      },
    },
  },
  /**
   * A function that returns a stroke object.           
   * @returns {object} - the stroke object.           
   */
  stroke: {
    curve: "straight",
  },
  /**
   * The markers object for the map.           
   * @property {number} size - The size of the markers.           
   */
  markers: {
    size: 5,
  },
  /**
   * An array of colors to use for the color picker.
   * @returns An array of colors to use for the color picker.
   */
  colors: ["#64B5F6", "#3273DC"],
  /**
   * A simple object that defines a gradient fill.           
   * @param {string} type - The type of gradient fill.           
   * @param {string[]} colors - The colors of the gradient fill.           
   * @returns None           
   */
  fill: {
    type: "gradient",
    colors: ["#64B5F6", "#3273DC"],
  },
  /**
   * A grid object that can be passed to the plot function.           
   * @property {boolean} show - Whether or not to show the grid.           
   * @property {boolean} xaxis - Whether or not to show the x-axis lines.           
   * @property {boolean} yaxis - Whether or not to show the y-axis lines.           
   */
  grid: {
    show: true,
    xaxis: {
      lines: {
        show: true,
      },
    },
    yaxis: {
      lines: {
        show: true,
      },
    },
  },
  /**
   * The title of the chart.           
   * @type {Object}           
   * @property {string} text - The text of the title.           
   * @property {string} align - The alignment of the title.           
   */
  title: {
    text: "",
    align: "left",
  },
  /**
   * A subtitle object that can be used to add a subtitle to a page.           
   * @param {string} text - The text of the subtitle.           
   * @param {string} [align="left"] - The alignment of the subtitle.           
   * @returns None           
   */
  subtitle: {
    text: "",
    align: "left",
  },
  /**
   * A function that returns a dataLabels object.           
   * @returns {object} - a dataLabels object.           
   */
  dataLabels: {
    enabled: false,
  },
  /**
   * The legend for the chart.           
   * @type {object}           
   */
  legend: {
    position: "top",
  },
  /**
   * The x-axis of the chart.           
   * @param {number} decimalsInFloat - the number of decimals to round the x-axis labels to.           
   * @param {string} title - the title of the x-axis.           
   * @returns None           
   */
  xaxis: {
    decimalsInFloat: 2,
    title: {
      text: "Stage",
    },
  },
  /**
   * Creates a ApexCharts configuration object for a line chart.           
   * @param {string} title - The title of the chart.           
   * @param {string} subtitle - The subtitle of the chart.           
   * @param {string} xAxisTitle - The title of the x-axis.           
   * @param {string} yAxisTitle - The title of the y-axis.           
   * @param {string} yAxisOppositeTitle - The title of the opposite y-axis.           
   * @param {string} yAxisOppositeTitle - The title of the opposite y-axis.           
   * @param {string} yAxisOpp
   */
  yaxis: [
    {
      title: {
        text: "Mittlere Geschwindigkeit",
      },
      labels: {
        formatter: (value) => {
          if(isFloat(value)) {
            return value.toFixed(2) + " m/s";
          } else {
            return value + " m/s";
          }
        },
      },
      forceNiceScale: true,
    },
    {
      title: {
        text: "Mittlere Beschleunigung",
      },
      opposite: true,
      labels: {
        formatter: (value) => {
          if(isFloat(value)) {
            return value.toFixed(2) + " m/s" + String.fromCharCode(178);
          } else {
            return value + " m/s" + String.fromCharCode(178);
          }
        },
      },
      forceNiceScale: true,
    },
  ],
};
