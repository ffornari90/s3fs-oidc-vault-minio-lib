pipeline {
    environment {
        s3fsImage = 'ffornari/s3fs-oidc-vault-minio'
        registryCredential = 'dockerhub'
        gitCredential = 'baltig'
        BUILD_VERSION = 'latest'
        OIDC_CLIENT_NAME = 'jenkins'
        VAULT_HOST = 'vault-minio-ns.apps.cnsa.cr.cnaf.infn.it'
        VAULT_PORT = '443'
        VAULT_ROLE = 'indigo'
        VAULT_TLS_ENABLE = 'true'
        VAULT_TLS_VERIFY = 'false'
        MINIO_HOST = 'minio-api-minio-ns.apps.cnsa.cr.cnaf.infn.it'
    }
    
    agent any
    
    stages {
        stage('Initial update status on gitlab') {
            steps {
                echo 'Notify GitLab'
                updateGitlabCommitStatus name: 'build', state: 'pending'
            }
        }
        stage('Cloning git repo') {
            steps {
                script {
                    withCredentials([gitUsernamePassword(credentialsId: 'baltig')]) {
                        try {
                            sh "rm -rf s3fs-fuse-oidc-vault-minio-lib/ && git clone https://baltig.infn.it/fornari/s3fs-fuse-oidc-vault-minio-lib.git"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'clone', state: 'failed'
                            sh "exit 1"
                        }
                    }
                }
            }
        }
        stage('Building the library') {
            steps {
                script {
                    try {
                        sh "cd s3fs-fuse-oidc-vault-minio-lib/ && cmake -S . -B build && cd build && sudo make install"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'build', state: 'failed'
                        sh "exit 1"
                    }
                }
            }
        }
        stage('Testing the library') {
            steps {
                script {
                    try {
                        sh "cd s3fs-fuse-oidc-vault-minio-lib/build && ./oidc-vault-minio_test"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'build', state: 'failed'
                        sh "exit 1"
                    }
                }
            }
        }
        stage('Building docker images') {
            steps {
                script {
                    try {
                        sh "docker build -f s3fs-fuse-oidc-vault-minio-lib/docker/Dockerfile -t $s3fsImage:$BUILD_VERSION s3fs-fuse-oidc-vault-minio-lib/docker"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'build', state: 'failed'
                        sh "exit 1"
                    }
                }
            }
        }
        stage('Logging into docker hub') {
            steps {
                script {
                    withCredentials([usernamePassword(credentialsId: "$registryCredential", passwordVariable: 'Password', usernameVariable: 'User')]) {
                        try {
                            sh "docker login -u ${env.User} -p ${env.Password}"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'login', state: 'failed'
                            sh "exit 1"
                        }
                    }
                }
            }
        }
        stage('Deploying docker images') {
            steps {
                script {
                    try {
                        sh "docker push $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'push', state: 'failed'
                        sh "exit 1"
                    }
                }
            }
        }
        stage('Remove unused docker images') {
            steps {
                script {
                    try {
                        sh "docker rmi $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'remove', state: 'failed'
                        sh "exit 1"
                    }
                }
            }
        }
        stage('Final update status on gitlab') {
            steps {
                echo 'Notify GitLab'
                updateGitlabCommitStatus name: 'build', state: 'success'
            }
        }        
    }
}
