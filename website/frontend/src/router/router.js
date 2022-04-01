import * as VueRouter from "vue-router";

/**
 * A list of routes for the application.           
 * @returns {Array<RouteConfig>}           
 */
const routes = [
  {
    name: "Dashboard",
    path: "/",
    component: () => import("../view/Dashboard.vue"),
  },
  {
    name: "Archive",
    path: "/Archive",
    component: () => import("../view/Archive.vue"),
  },
  {
    name: "View",
    path: "/View",
    component: () => import("../view/ViewData.vue"),
  },
];

/**
 * Creates a Vue router object.           
 * @returns {VueRouter}           
 */
const router = VueRouter.createRouter({
  history: VueRouter.createWebHistory(),
  routes,
});

export default router;
