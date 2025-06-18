// https://nuxt.com/docs/api/configuration/nuxt-config
import Aura from '@primeuix/themes/aura';

export default defineNuxtConfig({
  css: ['~/assets/css/app.css'],
  compatibilityDate: '2025-05-15',
  devtools: { enabled: true },
  modules: [
    "@primevue/nuxt-module",
    "@nuxtjs/tailwindcss"
  ],
  primevue: {
    options: {
      theme: {
        preset: Aura
      }
    }
  }
})
