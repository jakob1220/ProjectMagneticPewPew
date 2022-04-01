<template>
  <aside class="menu aside-style box">
    <p class="menu-label">ProjectMagneticPewPew</p>
    <ul class="menu-list">
      <li>
        <router-link
          :to="{ name: 'Dashboard' }"
          v-bind:class="
            this.$route.fullPath == '/'
              ? 'is-active active-style'
              : ''
          "
        >
          <span class="icon">
            <i class="fa fa-sliders-h" />
          </span>
          <span> Dashboard </span>
        </router-link>
      </li>
      <li>
        <router-link
          :to="{ name: 'Archive' }"
          v-bind:class="
            this.$route.fullPath == '/Archive'
              ? 'is-active active-style'
              : ''
          "
        >
          <span class="icon">
            <i class="fa fa-archive" />
          </span>
          <span> Archiv </span>
        </router-link>
      </li>
    </ul>
  </aside>
</template>

<script>
export default {
  name: "SideMenu",
};
</script>

<style>

.aside-style {
  position: fixed;
  width: 280px;
}
.active-style {
  background-color: 		hsl(207, 89%, 68%) !important;
}
</style>
