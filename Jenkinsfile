pipeline {
    environment {
        s3fsImage = 'ffornari/s3fs-oidc-vault-minio'
        registryCredential = 'dockerhub'
        gitCredential = 'baltig'
        BUILD_VERSION = 'latest'
        OIDC_CLIENT_NAME = 'jenkins-client'
        VAULT_HOST = 'vault-minio-ns.apps.cnsa.cr.cnaf.infn.it'
        VAULT_PORT = '443'
        VAULT_ROLE = 'indigo'
        VAULT_TLS_ENABLE = 'true'
        VAULT_TLS_VERIFY = 'false'
        MINIO_HOST = 'minio-api-minio-ns.apps.cnsa.cr.cnaf.infn.it'
    }
    
    agent any

    post {
        failure {
            updateGitlabCommitStatus name: 'clone', state: 'failed'
            updateGitlabCommitStatus name: 'build', state: 'failed'
            updateGitlabCommitStatus name: 'test', state: 'failed'
            updateGitlabCommitStatus name: 'image', state: 'failed'
            updateGitlabCommitStatus name: 'login', state: 'failed'
            updateGitlabCommitStatus name: 'push', state: 'failed'
            updateGitlabCommitStatus name: 'remove', state: 'failed'
        }
        success {
            updateGitlabCommitStatus name: 'clone', state: 'success'
            updateGitlabCommitStatus name: 'build', state: 'success'
            updateGitlabCommitStatus name: 'test', state: 'success'
            updateGitlabCommitStatus name: 'image', state: 'success'
            updateGitlabCommitStatus name: 'login', state: 'success'
            updateGitlabCommitStatus name: 'push', state: 'success'
            updateGitlabCommitStatus name: 'remove', state: 'success'
        }
    }

    stages {
        stage('start') {
            steps {
                echo 'Notify GitLab'
                updateGitlabCommitStatus name: 'clone', state: 'running'
            }
        }
        stage('clone') {
            steps {
                script {
                    withCredentials([gitUsernamePassword(credentialsId: 'baltig')]) {
                        try {
                            sh "sudo rm -rf s3fs-fuse-oidc-vault-minio-lib/ && git clone https://baltig.infn.it/fornari/s3fs-fuse-oidc-vault-minio-lib.git"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'clone', state: 'failed'
                            sh "exit 1"
                        }
                        updateGitlabCommitStatus name: 'clone', state: 'success'
                        updateGitlabCommitStatus name: 'build', state: 'running'
                    }
                }
            }
        }
        stage('build') {
            steps {
                script {
                    try {
                        sh "cd s3fs-fuse-oidc-vault-minio-lib/ && cmake -S . -B build && cd build && sudo make install"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'build', state: 'failed'
                        sh "exit 1"
                    }
                    updateGitlabCommitStatus name: 'build', state: 'success'
                    updateGitlabCommitStatus name: 'test', state: 'running'
                }
            }
        }
        stage('test') {
            steps {
                script {
                    try {
                        sh "cd s3fs-fuse-oidc-vault-minio-lib/build && eval \$(oidc-agent-service use) && ./oidc-vault-minio_test"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'test', state: 'failed'
                        sh "exit 1"
                    }
                    updateGitlabCommitStatus name: 'test', state: 'success'
                    updateGitlabCommitStatus name: 'image', state: 'running'
                }
            }
        }
        stage('image') {
            steps {
                script {
                    try {
                        sh "docker build -f s3fs-fuse-oidc-vault-minio-lib/docker/Dockerfile -t $s3fsImage:$BUILD_VERSION s3fs-fuse-oidc-vault-minio-lib/docker"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'image', state: 'failed'
                        sh "exit 1"
                    }
                    updateGitlabCommitStatus name: 'image', state: 'success'
                    updateGitlabCommitStatus name: 'login', state: 'running'
                }
            }
        }
        stage('login') {
            steps {
                script {
                    withCredentials([usernamePassword(credentialsId: "$registryCredential", passwordVariable: 'Password', usernameVariable: 'User')]) {
                        try {
                            sh "docker login -u ${env.User} -p ${env.Password}"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'login', state: 'failed'
                            sh "exit 1"
                        }
                        updateGitlabCommitStatus name: 'login', state: 'success'
                        updateGitlabCommitStatus name: 'push', state: 'running'
                    }
                }
            }
        }
        stage('push') {
            steps {
                script {
                    try {
                        sh "docker push $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'push', state: 'failed'
                        sh "exit 1"
                    }
                    updateGitlabCommitStatus name: 'push', state: 'success'
                    updateGitlabCommitStatus name: 'remove', state: 'running'
                }
            }
        }
        stage('remove') {
            steps {
                script {
                    try {
                        sh "docker rmi $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'remove', state: 'failed'
                        sh "exit 1"
                    }
                    updateGitlabCommitStatus name: 'remove', state: 'success'
                }
            }
        }
    }
}
