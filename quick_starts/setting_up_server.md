
# Website Hosting Setup Guide

This guide provides step-by-step instructions to set up a web server, configure DNS, enable HTTPS, and use Cloudflare for domain protection and performance.

---

## 1. Initial Server Setup

```bash
# Update system packages
apt update && apt upgrade -y   # Ubuntu/Debian

# Install web server (Nginx recommended)
apt install nginx -y
systemctl enable nginx
systemctl start nginx

# Test web server
curl http://localhost
```

---

## 2. Create a User with Sudo Access

```bash
# Create a new user
adduser yourusername

# Grant sudo privileges
usermod -aG sudo yourusername

# Optional: Disable root SSH login
nano /etc/ssh/sshd_config
# Set: PermitRootLogin no
systemctl restart ssh
```

---

## 3. Configure Firewall (UFW)

```bash
ufw allow OpenSSH         # Allow SSH
ufw allow 'Nginx Full'    # Allow HTTP and HTTPS
ufw enable                # Start firewall
```

---

## 4. Point Domain to Server

- Go to your domain registrar's DNS settings
- Set A and CNAME records:

```
@       A       your.server.ip.address
www     CNAME   @
```

- Update nameservers to Cloudflare (from their dashboard)

---

## 5. Configure Cloudflare DNS

- In Cloudflare DNS panel:

```
A     @       your.server.ip.address       (Proxied)
CNAME www     @                            (Proxied)
```

- Remove `_domainconnect` if no longer using the registrar’s DNS service

---

## 6. Configure Web Server (Nginx)

```bash
# Create web root
mkdir -p /var/www/sitename
echo "<h1>It works</h1>" > /var/www/sitename/index.html

# Create and edit Nginx server block
nano /etc/nginx/sites-available/sitename
```

Paste this:

```nginx
server {
    listen 80;
    server_name example.com www.example.com;
    root /var/www/sitename;
    index index.html;
}
```

```bash
# Enable config
ln -s /etc/nginx/sites-available/sitename /etc/nginx/sites-enabled/
nginx -t
systemctl reload nginx
```

---

## 7. Enable HTTPS with Let’s Encrypt

```bash
apt install certbot python3-certbot-nginx -y
certbot --nginx -d example.com -d www.example.com
```

---

## 8. Verify and Troubleshoot

```bash
# Check DNS resolution
dig example.com +short

# Check HTTP response
curl -I http://example.com

# Flush DNS cache if needed
# macOS:
sudo dscacheutil -flushcache; sudo killall -HUP mDNSResponder

# Windows:
ipconfig /flushdns
```

---
