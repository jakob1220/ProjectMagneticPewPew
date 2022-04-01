import App from "./App.vue";
import { createApp } from "vue";
import router from "./router/router.js";
import VueApexCharts from "vue3-apexcharts";
import "@fortawesome/fontawesome-free/js/all";


const app = createApp(App); // Initialize the Vue app.
app.use(router); 
app.use(VueApexCharts);
app.mount("#app"); //Mount the app to the DOM.  
