<template>
  <div id="app">
    <div class="header">
      <NavBar class="nav-style"></NavBar>
    </div>
    <div class="grid-parent">
      <div class="grid-side p-3">
        <SideMenu></SideMenu>
      </div>
      <div class="grid-content p-3">
        <router-view />
      </div>
    </div>
  </div>
</template>

<script>
import SideMenu from "./components/SideMenu.vue";
import NavBar from "./components/NavBar.vue";

export default {
  name: "App",
  components: {
    SideMenu,
    NavBar,
  },
  data() {
    return {};
  },
};
</script>

<style>
@import "~bulma/css/bulma.css";
@import "~bulma-prefers-dark/css/bulma-prefers-dark.css";

.header {
  position: fixed;
  left: 0;
  top: 0;
  width: 100%;
  z-index: 200;
  height: 90px;
}

.grid-parent {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr 1fr 1fr;
  grid-template-rows: 1fr;
  gap: 0px 0px;
  grid-template-areas: "side content content content content";
  margin-top: 90px;
}

.grid-side {
  grid-area: side;
}

.grid-content {
  grid-area: content;
}
</style>
