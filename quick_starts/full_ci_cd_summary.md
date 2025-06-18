# 🚀 Static Site CI/CD Deployment Summary

This document summarizes how to set up continuous deployment of static websites using GitHub Actions, Vue, Nuxt, Tailwind CSS, PrimeVue, and SSH deployment via rsync.

---

## 🧩 1. Simple Static Site Deployment via GitHub Actions

### ✔️ Assumptions:
- Static files are hosted on a Linux server
- SSH access is configured
- `DEPLOY_KEY`, `DEPLOY_USER`, `DEPLOY_HOST`, `DEPLOY_PATH` secrets exist in GitHub

### ✅ GitHub Actions Workflow:
```yaml
name: Deploy Static Website

on:
  push:
    branches:
      - master

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Set up SSH
        run: |
          mkdir -p ~/.ssh
          echo "${{ secrets.DEPLOY_KEY }}" > ~/.ssh/id_rsa
          chmod 600 ~/.ssh/id_rsa
          ssh-keyscan -H ${{ secrets.DEPLOY_HOST }} >> ~/.ssh/known_hosts
      - name: Deploy via rsync
        run: |
          rsync -avz --delete ./ ${{ secrets.DEPLOY_USER }}@${{ secrets.DEPLOY_HOST }}:${{ secrets.DEPLOY_PATH }}
```

---

## 🔐 SSH Setup Instructions

1. Generate key:
```bash
ssh-keygen -t rsa -b 4096 -C "deploy@yourdomain.com"
```

2. Copy public key to server:
```bash
ssh-copy-id -i ~/.ssh/github-actions.pub user@server
```

3. Add the **private key** as `DEPLOY_KEY` in GitHub secrets.

---

## 🛠️ Handling rsync Permission Denied Errors

- Use `chown -R user:user /path/to/site` to fix file ownership
- Use `--rsync-path="sudo rsync"` and configure passwordless sudo if needed
- Recommended: Deploy to a dedicated writable directory

---

## 🌐 Vue + Vue Router Notes

- Use `createWebHistory()` in Vue Router
- Add catch-all routing on server:

### Nginx:
```nginx
location / {
  try_files $uri $uri/ /index.html;
}
```

### Apache (.htaccess):
```apache
<IfModule mod_rewrite.c>
  RewriteEngine On
  RewriteBase /
  RewriteRule ^index\.html$ - [L]
  RewriteCond %{REQUEST_FILENAME} !-f
  RewriteCond %{REQUEST_FILENAME} !-d
  RewriteRule . /index.html [L]
</IfModule>
```

---

## ⚙️ Nuxt 3 + Tailwind CSS + PrimeVue Static Deployment

### Project Setup:
```bash
npx nuxi init my-app
cd my-app
npm install
```

### Tailwind Setup:
```bash
npm install -D tailwindcss postcss autoprefixer
npx tailwindcss init -p
```

### PrimeVue:
```bash
npm install primevue@^3.50.0 primeicons
```

Create plugin and configure theme in `nuxt.config.ts`.

### nuxt.config.ts:
```ts
export default defineNuxtConfig({
  ssr: true,
  target: 'static',
  nitro: { preset: 'static' },
  css: [
    'primevue/resources/themes/lara-light-indigo/theme.css',
    'primevue/resources/primevue.min.css',
    'primeicons/primeicons.css',
    '@/assets/css/tailwind.css'
  ],
  build: {
    transpile: ['primevue'],
  },
})
```

### Static Build:
```bash
npm run generate
```

Deploy `.output/public/` using GitHub Actions with rsync.

---

## 🔒 Required GitHub Secrets

| Name           | Description                     |
|----------------|---------------------------------|
| `DEPLOY_KEY`   | SSH private key                 |
| `DEPLOY_USER`  | Server SSH username             |
| `DEPLOY_HOST`  | Server IP or domain             |
| `DEPLOY_PATH`  | Directory to deploy static files|

---

Let me know if you need Docker, SSR, database-backed, or cloud-provider specific setups.
