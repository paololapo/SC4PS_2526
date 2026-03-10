# CloudVeneto Linux Setup Guide

### Compiling and Running C Programs

This guide explains how to create and configure a **Linux virtual machine on CloudVeneto** and prepare it for compiling and running **C programs using GCC**.

The steps include:

1. Accessing the CloudVeneto dashboard
2. Creating an SSH key pair
3. Launching a Linux virtual machine
4. Connecting through the CloudVeneto gate server
5. Installing development tools
6. Compiling and running C code

---

# 1. Access the CloudVeneto Dashboard

Open the CloudVeneto dashboard:

```
https://cloudveneto.ict.unipd.it/dashboard
```

Login using one of the available authentication methods:

* **INFN account (AAI)**
* **University of Padova account (SSO)**
* **Direct username and password**

After logging in, select your project (e.g. **SC4PS-PhD**) from the top menu. 

---

# 2. Create an SSH Key Pair

To securely access the virtual machine you must create an **SSH key pair**.

### Steps

1. Navigate to:

```
Project → Compute → Key Pairs
```

2. Click **Create Key Pair**

3. Choose a name for the key
   Example:

```
cloudveneto-key
```

4. Download the **private key (.pem)** file.

Move the key to a secure location and set correct permissions:

```bash
chmod 600 cloudveneto-key.pem
```

SSH will refuse to use keys with insecure permissions.

---

# 3. Launch a Linux Virtual Machine

Create a new instance.

Navigate to:

```
Project → Compute → Instances → Launch Instance
```

Configure the instance with the following parameters.

### Instance Settings

**Details**

```
Instance Name: my_vm_name
```

**Source**

```
Boot Source: Image
Create New Volume: No
Image: AlmaLinux10-INFNPadova
```

**Flavor**

Choose a configuration providing sufficient memory and CPU.

Example:

```
cldareapd.large
```

(typically ~8 GB RAM) 

**Key Pair**

Select the key created earlier.

**Network**

Use the default project network:

```
<ProjectName>-lan
```

Click **Launch Instance**.

After a short time the instance status becomes **Active** and an **IP address** will appear in the instance list.

---

# 4. Connect to the CloudVeneto Gate Server

If you are **outside the university network**, you must first connect to the **CloudVeneto gate server**.

```bash
ssh username@gate.cloudveneto.it
```

You will receive your **username and initial password** by email when your CloudVeneto account is approved. 

---

# 5. Copy the SSH Key to the Gate Machine

From your **local machine**, copy your private key to the gate server:

```bash
scp cloudveneto-key.pem username@gate.cloudveneto.it:~
```

Then organize the key on the gate server:

```bash
mkdir ~/private
mv ~/cloudveneto-key.pem ~/private/
chmod 600 ~/private/cloudveneto-key.pem
```

---

# 6. Connect to the Virtual Machine

Use the **private IP address** of the instance.

For **AlmaLinux images**, the default user is:

```
almalinux
```

Connect using:

```bash
ssh -i ~/private/cloudveneto-key.pem almalinux@<VM_IP_ADDRESS>
```

Example:

```bash
ssh -i ~/private/cloudveneto-key.pem almalinux@10.67.20.144
```

---

# 7. Configure SSH Shortcut

To simplify access, you can configure SSH aliases.

Edit your SSH configuration file:

```bash
nano ~/.ssh/config
```

Add:

```bash
Host cloudveneto-gate
    HostName gate.cloudveneto.it
    User username

Host cloudvm
    HostName <VM_IP_ADDRESS>
    User almalinux
    IdentityFile ~/.ssh/cloudveneto-key.pem
    ProxyJump cloudveneto-gate
```

Set correct permissions:

```bash
chmod 600 ~/.ssh/config
```

Now you can connect simply with:

```bash
ssh cloudvm
```

---

# 8. Update the System

Once logged into the VM, update the package manager:

```bash
sudo dnf check-update
sudo dnf upgrade -y
```

---

# 9. Install Development Tools

To compile C programs you must install **GCC and development utilities**.

Install the development tools group:

```bash
sudo dnf groupinstall "Development Tools" -y
```

Optionally enable additional repositories:

```bash
sudo dnf config-manager --set-enabled crb
```

Verify the compiler installation:

```bash
gcc --version
```

If the version number is displayed, the compiler is correctly installed. 

---

# 10. Install a Text Editor

To write code you can install a terminal editor such as **nano**:

```bash
sudo dnf install nano
```

---

# 11. Write a Simple C Program

Create a new file:

```bash
nano hello.c
```

Insert the following program:

```c
#include <stdio.h>

int main() {
    printf("Hello CloudVeneto!\n");
    return 0;
}
```

Save and exit:

```
CTRL + O
Enter
CTRL + X
```

---

# 12. Compile the Program

Compile the source file using GCC:

```bash
gcc hello.c -o hello
```

Explanation:

| Command    | Meaning           |
| ---------- | ----------------- |
| `gcc`      | GNU C compiler    |
| `hello.c`  | source file       |
| `-o hello` | output executable |

---

# 13. Run the Program

Execute the compiled program:

```bash
./hello
```

Expected output:

```
Hello CloudVeneto!
```

If the message appears, the **CloudVeneto environment is correctly configured for C development**.

